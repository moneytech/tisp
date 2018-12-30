/* See LICENSE file for copyright and license details. */
#include <stdio.h>

#include "../tisp.h"

#define PRIM_OP(NAME, OP, FUNC)                                                        \
static Val                                                                             \
prim_##NAME(Env env, Val args)                                                         \
{                                                                                      \
	Val a, b;                                                                      \
	tsp_arg_num(args, FUNC, 2);                                                    \
	if (!(a = tisp_eval(env, car(args))) ||                                        \
	    !(b = tisp_eval(env, car(cdr(args)))))                                     \
		return NULL;                                                           \
	tsp_arg_type(a, FUNC, INTEGER);                                                \
	tsp_arg_type(b, FUNC, INTEGER);                                                \
	return mk_int(a->v.i OP b->v.i);                                               \
}

PRIM_OP(add, +, "+")
PRIM_OP(mul, *, "*")
PRIM_OP(mod, %, "mod")

static Val
prim_sub(Env env, Val args)
{
	Val a, b;
	int len = list_len(args);
	if (len != 2 && len != 1)
		tsp_warnf("-: expected 1 or 2 arguments, recieved %d", len);
	if (!(a = tisp_eval(env, car(args))))
		return NULL;
	tsp_arg_type(a, "-", INTEGER);
	if (len == 1)
		return mk_int(-a->v.i);
	if (!(b = tisp_eval(env, car(cdr(args)))))
		return NULL;
	tsp_arg_type(b, "-", INTEGER);
	return mk_int(a->v.i - b->v.i);
}

static Val
prim_div(Env env, Val args)
{
	Val a, b;
	tsp_arg_num(args, "/", 2);
	if (!(a = tisp_eval(env, car(args))) || !(b = tisp_eval(env, car(cdr(args)))))
		return NULL;
	tsp_arg_type(a, "/", INTEGER);
	tsp_arg_type(b, "/", INTEGER);
	return mk_rat(a->v.i, b->v.i);
}

#define PRIM_COMPARE(NAME, OP, FUNC)                                  \
static Val                                                            \
prim_##NAME(Env env, Val args)                                        \
{                                                                     \
	Val v;                                                        \
	if (!(v = tisp_eval_list(env, args)))                         \
		return NULL;                                          \
	if (list_len(v) != 2)                                         \
		return env->t;                                        \
	tsp_arg_type(car(v), FUNC, INTEGER);                          \
	tsp_arg_type(car(cdr(v)), FUNC, INTEGER);                     \
	return (car(v)->v.i OP car(cdr(v))->v.i) ? env->t : env->nil; \
}

PRIM_COMPARE(lt,  <,  "<")
PRIM_COMPARE(gt,  >,  ">")
PRIM_COMPARE(lte, <=, "<=")
PRIM_COMPARE(gte, >=, ">=")

void
tib_env_math(Env env)
{
	tisp_env_add(env, "+",   mk_prim(prim_add));
	tisp_env_add(env, "-",   mk_prim(prim_sub));
	tisp_env_add(env, "*",   mk_prim(prim_mul));
	tisp_env_add(env, "/",   mk_prim(prim_div));
	tisp_env_add(env, "mod", mk_prim(prim_mod));
	tisp_env_add(env, "<",   mk_prim(prim_lt));
	tisp_env_add(env, ">",   mk_prim(prim_gt));
	tisp_env_add(env, "<=",  mk_prim(prim_lte));
	tisp_env_add(env, ">=",  mk_prim(prim_gte));
}
