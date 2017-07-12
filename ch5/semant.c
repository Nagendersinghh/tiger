#include <stdio.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "trans.h"
#include "env.h"
#include "semant.h"

static struct expty transExp(S_table venv, S_table tenv, A_exp exp);
static void transDec(S_table venv, S_table tenv, A_dec dec);
static struct expty transVar(S_table venv, S_table tenv, A_var var);
static Ty_ty transTy(S_table tenv, A_ty ty);
static Ty_ty actual_ty(Ty_ty ty);
static int match_ty(Ty_ty f, Ty_ty s);
static int match_argList(S_table venv, S_table tenv, Ty_tyList formals, A_expList exp);
static int match_efieldList(S_table venv, S_table tenv, Ty_fieldList fl, A_efieldList el);
static Ty_tyList makeFormalTyList(S_table tenv, A_fieldList formals); 
static Ty_fieldList makeFieldListTys(S_table tenv, A_fieldList fields);

void SEM_transProg(A_exp prog) {
	struct expty pgrm;

	S_table t_env = E_base_tenv();
	S_table v_env = E_base_venv();	

	pgrm = transExp(v_env, t_env, prog);
}

static Ty_ty actual_ty(Ty_ty ty) {
	if (!ty)
		return ty;
	if(ty->kind == Ty_name) return actual_ty(ty->u.name.ty);
	return ty;
}

static int match_ty(Ty_ty f, Ty_ty s) {
	Ty_ty first = actual_ty(f);
	Ty_ty second = actual_ty(s);
	/* Every record can be of type nil */
	if (first->kind == Ty_record && second->kind == Ty_nil)
		return 1;
	if (first->kind == Ty_nil && second->kind == Ty_record)
		return 1;
	/* If record or array types, check for pointer equality */
	if (first->kind == Ty_record || first->kind == Ty_array)
		return first == second;
	return first->kind == second->kind;
}

static int match_argList(S_table venv, S_table tenv, Ty_tyList formals, A_expList args) {
	Ty_tyList fl = formals;
	A_expList ag = args;
	while(fl && ag) {
		struct expty arg = transExp(venv, tenv, args->head);
		if(!match_ty(arg.ty, formals->head)) {
			return 0;
		}
		fl = fl->tail;
		ag = ag->tail;
	}
	if ((fl && !ag) || (!fl && ag)) {
		return 0;
	}
	return 1;
}

static int match_efieldList(S_table venv, S_table tenv, Ty_fieldList fl, A_efieldList el) {
	Ty_fieldList fll = fl;
	A_efieldList ell = el;
	while(fll && ell) {
		struct expty ex = transExp(venv, tenv, ell->head->exp);
		if(!match_ty(fll->head->ty, ex.ty) || fll->head->name != ell->head->name) {
			return 0;
		}
		fll = fll->tail;
		ell = ell->tail;
	}
	if ((ell && !fll) || (!ell && fll))
		return 0;
	return 1;
}

static struct expty transExp(S_table venv, S_table tenv, A_exp exp) {
	switch(exp->kind) {
		case A_varExp:
			return transVar(venv, tenv, exp->u.var);
		case A_nilExp:
			return expTy(NULL, Ty_Nil());
		case A_intExp:
			return expTy(NULL, Ty_Int());
		case A_stringExp:
			return expTy(NULL, Ty_String());
		case A_callExp: {
			E_enventry fun = S_look(venv, exp->u.call.func);
			if (!fun || fun->kind != E_funEntry) {
				char msg[80];
				sprintf(msg, "function %s is not defined\n", S_name(exp->u.call.func));
				EM_error(exp->pos, msg);
				return expTy(NULL, Ty_Void());
			}
			if(match_argList(venv, tenv, fun->u.fun.formals, exp->u.call.args)) {
				if(fun->u.fun.result) {
					return expTy(NULL, actual_ty(fun->u.fun.result));
				} else {
					return expTy(NULL, Ty_Void());
				}
			}
			EM_error(exp->pos, "Function parameters don't match\n");
			return expTy(NULL, Ty_Void());
		}
		case A_opExp: {
			A_oper oper = exp->u.op.oper;
			struct expty left = transExp(venv, tenv, exp->u.op.left);
			struct expty right = transExp(venv, tenv, exp->u.op.right);

			if(left.ty->kind == Ty_record || right.ty->kind == Ty_record) {
				if(left.ty->kind == Ty_record && right.ty->kind == Ty_nil) {
					return expTy(NULL, Ty_Int());
				}
				if(right.ty->kind == Ty_record && left.ty->kind == Ty_nil) {
					return expTy(NULL, Ty_Int());
				}
			}
			if(!match_ty(left.ty, right.ty)) {
				EM_error(exp->pos, "Type mismatch\n");
			}
			return expTy(NULL, Ty_Int());
		}
		case A_recordExp: {
			Ty_ty rec = actual_ty(S_look(tenv, exp->u.record.typ));
			if (!rec || rec->kind != Ty_record) {
				char msg[80];
				sprintf(msg, "Type %s is not defined\n", S_name(exp->u.record.typ));
				EM_error(exp->pos, msg);
				return expTy(NULL, Ty_Record(NULL));
			}
			if(match_efieldList(venv, tenv, rec->u.record, exp->u.record.fields))
				return expTy(NULL, rec);

			EM_error(exp->pos, "Fields for record do not match the definition\n");
			return expTy(NULL, Ty_Record(NULL));
		}
		case A_seqExp: {
			A_expList list = exp->u.seq;
			struct expty final;
			if(!list) {
				return expTy(NULL, Ty_Void());
			}
			while(list->tail) {
				transExp(venv, tenv, list->head);
				list = list->tail;
			}
			return final = transExp(venv, tenv, list->head);
		}
		case A_assignExp: {
			struct expty var = transVar(venv, tenv, exp->u.assign.var);
			struct expty val = transExp(venv, tenv, exp->u.assign.exp);
			if (match_ty(var.ty, val.ty))
				return expTy(NULL, Ty_Void());
			EM_error(exp->pos, "Types don't match\n");
			return expTy(NULL, Ty_Void());
		}
		case A_ifExp: {
			struct expty test = transExp(venv, tenv, exp->u.iff.test);
			struct expty then;
			struct expty elsee;
			if (test.ty->kind != Ty_int) {
				EM_error(exp->u.iff.test->pos, "Integer required\n");
			}
			then = transExp(venv, tenv, exp->u.iff.then);
			if (exp->u.iff.elsee) {
				elsee = transExp(venv, tenv, exp->u.iff.elsee);
				if (!match_ty(elsee.ty, then.ty)) {
					EM_error(exp->pos, "Then and else types must be the same\n");
					return expTy(NULL, Ty_Void());
				}
				return expTy(NULL, then.ty);
			}
			if (then.ty->kind != Ty_void) {
				EM_error(exp->u.iff.then->pos, "Then should be of type void\n");
				return expTy(NULL, Ty_Void());
			}
			return expTy(NULL, then.ty);
		}
		case A_whileExp: {
			struct expty test = transExp(venv, tenv, exp->u.whilee.test);
			struct expty body = transExp(venv, tenv, exp->u.whilee.body);
			if (test.ty->kind != Ty_int) {
				EM_error(exp->u.whilee.test->pos, "Integer required\n");
			}
			if (body.ty->kind != Ty_void) {
				EM_error(exp->u.whilee.body->pos, "While should be of void type\n");
			}
			return expTy(NULL, Ty_Void());
		}
		case A_forExp: {
			struct expty hi = transExp(venv, tenv, exp->u.forr.hi);
			struct expty lo = transExp(venv, tenv, exp->u.forr.lo);
			struct expty body;

			if (lo.ty->kind != Ty_int || hi.ty->kind != Ty_int) {
				EM_error(exp->pos, "Lower and upper range should be an int\n");
			}
			S_beginScope(venv);
			transDec(venv, tenv, A_VarDec(exp->u.forr.body->pos, exp->u.forr.var, S_Symbol("int"), exp->u.forr.lo));
			body = transExp(venv, tenv, exp->u.forr.body);
			S_endScope(venv);
			if (body.ty->kind != Ty_void) {
				EM_error(exp->u.forr.body->pos, "Body of for should be of type void\n");
			}
			return expTy(NULL, Ty_Void());
		}
		case A_breakExp:
			return expTy(NULL, Ty_Void());
		case A_letExp: {
			struct expty body;
			A_decList d = exp->u.let.decs;
			S_beginScope(venv);
			S_beginScope(tenv);
			while(d) {
				transDec(venv, tenv, d->head);
				d = d->tail;
			}
			body = transExp(venv, tenv, exp->u.let.body);
			S_endScope(tenv);
			S_endScope(venv);
			return body;
		}
		case A_arrayExp: {
			Ty_ty typ = actual_ty(S_look(tenv, exp->u.array.typ));
			struct expty size = transExp(venv, tenv, exp->u.array.size);
			struct expty init = transExp(venv, tenv, exp->u.array.init);
			if (!typ || typ->kind != Ty_array) {
				EM_error(exp->pos, "Type not defined\n");
			}
			if (size.ty->kind != Ty_int) {
				EM_error(exp->u.array.size->pos, "Expected integer\n");
			}
			if (!match_ty(init.ty, typ->u.array)) {
				EM_error(exp->pos, "Unmatched array type\n");
			}
			return expTy(NULL, typ);
		}
		default:
			assert(0);
	}
}

static struct expty transVar(S_table venv, S_table tenv, A_var var) {
	switch(var->kind) {
		case A_simpleVar: {
			E_enventry variable = S_look(venv, var->u.simple);
			if(!variable || variable->kind != E_varEntry) {
				EM_error(var->pos, "Undefined variable %s\n", S_name(var->u.simple));
				return expTy(NULL, Ty_Int());
			}
			return expTy(NULL, actual_ty(variable->u.var.ty));
		}
		case A_fieldVar: {
			Ty_fieldList fl;
			struct expty record = transVar(venv, tenv, var->u.field.var);
			if(record.ty->kind != Ty_record) {
				EM_error(var->pos, "Not a record type\n");
				return expTy(NULL, Ty_Record(NULL));
			}
			for(fl = record.ty->u.record; fl; fl = fl->tail) {
				if(fl->head->name == var->u.field.sym) {
					return expTy(NULL, actual_ty(fl->head->ty));
				}
			}
			EM_error(var->pos, "No such type in the field\n");
			return expTy(NULL, Ty_Record(NULL));
		}
		case A_subscriptVar: {
			struct expty arr = transVar(venv, tenv, var->u.subscript.var);
			struct expty e = transExp(venv, tenv, var->u.subscript.exp);
			if(arr.ty->kind != Ty_array) {
				EM_error(var->pos, "Not an array type\n");
				return expTy(NULL, Ty_Array(NULL));
			}
			if(e.ty->kind != Ty_int) {
				EM_error(var->u.subscript.exp->pos, "Integer required\n");
				return expTy(NULL, Ty_Array(NULL));
			}
			return expTy(NULL, actual_ty(arr.ty->u.array));
		}
		default: assert(0);
	}
}

static void transDec(S_table venv, S_table tenv, A_dec dec) {
	switch(dec->kind) {
		case A_varDec: {
			struct expty exp = transExp(venv, tenv, dec->u.var.init);
			S_enter(venv, dec->u.var.var, E_VarEntry(exp.ty));
			break;
		}
		case A_typeDec: {
			A_nametyList nl;
			Ty_ty nameTy;
			int isCyclic = 1;
			for(nl = dec->u.type; nl; nl = nl->tail) {
				S_enter(tenv, nl->head->name, Ty_Name(nl->head->name, NULL));
			}
			for(nl = dec->u.type; nl; nl = nl->tail) {
				Ty_ty ty = transTy(tenv, nl->head->ty);
				if(isCyclic) {
					if(ty->kind != Ty_name)
						isCyclic = 0;
				}
				nameTy = S_look(tenv, nl->head->name);
				nameTy->u.name.ty = ty;
			}
			if(isCyclic) {
				EM_error(dec->pos, "Cyclic type definitions\n");
			}
			return;
		}
		case A_functionDec: {
			A_fundecList fl;
			int isCyclic = 1;
			for(fl = dec->u.function; fl; fl = fl->tail) {
				Ty_ty resultTy = NULL;
				if(fl->head->result) {
					resultTy = S_look(tenv, fl->head->result);
					if(!resultTy) {
						EM_error(fl->head->pos, "return type undefined\n");
					}
				}
				if(!resultTy)
					resultTy = Ty_Void();

				Ty_tyList formalTys = makeFormalTyList(tenv, fl->head->params);
				S_enter(venv, fl->head->name, E_FunEntry(formalTys, resultTy));
			}
			for(fl = dec->u.function; fl; fl = fl->tail) {
				E_enventry func = S_look(venv, fl->head->name);
				A_fieldList params = NULL;
				Ty_tyList paramTys = NULL;
				S_beginScope(venv);
				for(params = fl->head->params, paramTys = func->u.fun.formals; params; params = params->tail, paramTys = paramTys->tail) {
					S_enter(venv, params->head->name, E_VarEntry(paramTys->head));
				}
				struct expty result = transExp(venv, tenv, fl->head->body);
				if(!match_ty(result.ty, func->u.fun.result)) {
					EM_error(fl->head->body->pos, "Type mismatch in function body\n");
				}
				S_endScope(venv);
			}
			return;
		}
		assert(0);
	}
}

static Ty_tyList makeFormalTyList(S_table tenv, A_fieldList formals) {
	Ty_tyList paramList;
	Ty_ty ty = NULL;
	if(!formals) {
		return NULL;
	}
	ty = S_look(tenv, formals->head->typ);
	if(!ty) {
		EM_error(formals->head->pos, "Undefined type name %s\n", S_name(formals->head->name));
		ty = Ty_Void();
	}
	paramList = Ty_TyList(Ty_Name(formals->head->name, ty), makeFormalTyList(tenv, formals->tail));
	return paramList;
}

static Ty_ty transTy(S_table tenv, A_ty ty) {
	switch(ty->kind) {
		case A_nameTy: {
			Ty_ty type = S_look(tenv, ty->u.name);
			if(!type) {
				EM_error(ty->pos, "Undefined type %s\n", S_name(ty->u.name));
				return Ty_Void();
			}
			return type;
		}
		case A_recordTy: {
			Ty_fieldList fieldTys = makeFieldListTys(tenv, ty->u.record);
			return Ty_Record(fieldTys);
		}
		case A_arrayTy: {
			Ty_ty type = S_look(tenv, ty->u.array);
			if(!type) {
				EM_error(ty->pos, "Undefined type %s\n", S_name(ty->u.name));
			}
			return Ty_Array(type);
		}
		default: assert(0);
	}
}

static Ty_fieldList makeFieldListTys(S_table tenv, A_fieldList fields) {
	Ty_fieldList fieldList;
	Ty_ty ty = NULL;
	if(!fields)
		return NULL;
	ty = S_look(tenv, fields->head->typ);
	if(!ty) {
		EM_error(fields->head->pos, "Undefined type\n");
		ty = Ty_Void();
	}
	fieldList = Ty_FieldList(Ty_Field(fields->head->name, ty), makeFieldListTys(tenv, fields->tail));
	return fieldList;
}

