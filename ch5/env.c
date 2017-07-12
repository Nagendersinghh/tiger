#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "types.h"
#include "env.h"

E_enventry E_VarEntry(Ty_ty ty) {
	E_enventry var = checked_malloc(sizeof(struct E_enventry_));
	var->kind = E_varEntry;
	var->u.var.ty = ty;
	return var;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result) {
	E_enventry fun = checked_malloc(sizeof(struct E_enventry_));

	fun->kind = E_funEntry;
	fun->u.fun.formals = formals;
	fun->u.fun.result = result;
	return fun;
}

S_table E_base_tenv(void) {
	S_table init = S_empty();
	S_enter(init, S_Symbol("int"), Ty_Int());
	S_enter(init, S_Symbol("string"), Ty_String());
	return init;
}

S_table E_base_venv(void) {
	S_table init = S_empty();
	S_enter(init, S_Symbol("print"), E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Void()));
	S_enter(init, S_Symbol("flush"), E_FunEntry(NULL, Ty_Void()));
	S_enter(init, S_Symbol("getchar"), E_FunEntry(NULL, Ty_String()));
	S_enter(init, S_Symbol("ord"), E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Int()));
	S_enter(init, S_Symbol("chr"), E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_String()));
	S_enter(init, S_Symbol("size"), E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Int()));
	S_enter(init, S_Symbol("substring"), E_FunEntry(Ty_TyList(Ty_String(), Ty_TyList(
						Ty_Int(), Ty_TyList(Ty_Int(), NULL))), Ty_String()));
	S_enter(init, S_Symbol("concat"), E_FunEntry(Ty_TyList(Ty_String(), Ty_TyList(
						Ty_String(), NULL)), Ty_String()));
	S_enter(init, S_Symbol("not"), E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_Int()));
	S_enter(init, S_Symbol("exit"), E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_Void()));
	return init;
}

