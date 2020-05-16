#ifndef GEN_ICOUNT_H
#define GEN_ICOUNT_H 1

#include "qemu/timer.h"

/* Helpers for instruction counting code generation.  */

static TCGArg *icount_arg;
static int icount_label;

static inline void gen_io_start(TCGContext *tcg_ctx)
{
    TCGv_i32 tmp = tcg_const_i32(tcg_ctx, 1);
    tcg_gen_st_i32(tcg_ctx, tmp, tcg_ctx->cpu_env,
                   offsetof(CPUState, can_do_io) - ENV_OFFSET);
    tcg_temp_free_i32(tcg_ctx, tmp);
}

/*
 * cpu->can_do_io is cleared automatically at the beginning of
 * each translation block.  The cost is minimal and only paid
 * for -icount, plus it would be very easy to forget doing it
 * in the translator.  Therefore, backends only need to call
 * gen_io_start.
 */
static inline void gen_io_end(TCGContext *tcg_ctx)
{
    TCGv_i32 tmp = tcg_const_i32(tcg_ctx, 0);
    tcg_gen_st_i32(tcg_ctx, tmp, tcg_ctx->cpu_env,
                   offsetof(CPUState, can_do_io) - ENV_OFFSET);
    tcg_temp_free_i32(tcg_ctx, tmp);
}

static inline void gen_tb_start(TCGContext *tcg_ctx, TranslationBlock *tb)
{
    TCGv_i32 count, flag;

    tcg_ctx->exitreq_label = gen_new_label(tcg_ctx);
    flag = tcg_temp_new_i32(tcg_ctx);
    tcg_gen_ld_i32(tcg_ctx, flag, tcg_ctx->cpu_env,
                   offsetof(CPUState, tcg_exit_req) - ENV_OFFSET);
    tcg_gen_brcondi_i32(tcg_ctx, TCG_COND_NE, flag, 0, tcg_ctx->exitreq_label);
    tcg_temp_free_i32(tcg_ctx, flag);

    if (!(tb->cflags & CF_USE_ICOUNT)) {
        return;
    }

    icount_label = gen_new_label(tcg_ctx);
    count = tcg_temp_local_new_i32(tcg_ctx);
    tcg_gen_ld_i32(tcg_ctx, count, tcg_ctx->cpu_env,
                   offsetof(CPUState, icount_decr.u32) - ENV_OFFSET);
    /* This is a horrid hack to allow fixing up the value later.  */
    icount_arg = tcg_ctx->gen_opparam_ptr + 1;
    tcg_gen_subi_i32(tcg_ctx, count, count, 0xdeadbeef);

    tcg_gen_brcondi_i32(tcg_ctx, TCG_COND_LT, count, 0, icount_label);
    tcg_gen_st16_i32(tcg_ctx, count, tcg_ctx->cpu_env,
                     offsetof(CPUState, icount_decr.u16.low) - ENV_OFFSET);
    tcg_temp_free_i32(tcg_ctx, count);
}

static inline void gen_tb_end(TCGContext *tcg_ctx, TranslationBlock *tb, int num_insns)
{
    gen_set_label(tcg_ctx, tcg_ctx->exitreq_label);
    tcg_gen_exit_tb(tcg_ctx, (uintptr_t)tb + TB_EXIT_REQUESTED);

    if (tb->cflags & CF_USE_ICOUNT) {
        *icount_arg = num_insns;
        gen_set_label(tcg_ctx, icount_label);
        tcg_gen_exit_tb(tcg_ctx, (uintptr_t)tb + TB_EXIT_ICOUNT_EXPIRED);
    }
}

#endif