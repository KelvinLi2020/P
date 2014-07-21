#include "PrtCmdPrinting.h"

void PrtCmdPrintType(_In_ PRT_TYPE type)
{
	PRT_TYPE_KIND kind = type.typeKind;
	switch (kind)
	{
	case PRT_KIND_ANY:
		printf_s("any");
		break;
	case PRT_KIND_BOOL:
		printf_s("bool");
		break;
	case PRT_KIND_EVENT:
		printf_s("event");
		break;
	case PRT_KIND_MACHINE:
		printf_s("machine");
		break;
	case PRT_KIND_INT:
		printf_s("int");
		break;
	case PRT_KIND_MODEL:
		printf_s("model");
		break;
	case PRT_KIND_FORGN:
		printf_s("foreign");
		break;
	case PRT_KIND_MAP:
	{
		PRT_MAPTYPE *mtype = (PRT_MAPTYPE *)type.typeUnion.map;
		printf_s("map[");
		PrtCmdPrintType(mtype->domType);
		printf_s(", ");
		PrtCmdPrintType(mtype->codType);
		printf_s("]");
		break;
	}
	case PRT_KIND_NMDTUP:
	{
		PRT_UINT32 i;
		PRT_NMDTUPTYPE *ntype = (PRT_NMDTUPTYPE *)type.typeUnion.nmTuple;
		printf_s("(");
		for (i = 0; i < ntype->arity; ++i)
		{
			printf_s("%s: ", ntype->fieldNames[i]);
			PrtCmdPrintType(ntype->fieldTypes[i]);
			if (i < ntype->arity - 1)
			{
				printf_s(", ");
			}
			else
			{
				printf_s(")");
			}
		}

		break;
	}
	case PRT_KIND_SEQ:
	{
		PRT_SEQTYPE *stype = (PRT_SEQTYPE *)type.typeUnion.seq;
		printf_s("seq[");
		PrtCmdPrintType(stype->innerType);
		printf_s("]");
		break;
	}
	case PRT_KIND_TUPLE:
	{
		PRT_UINT32 i;
		PRT_TUPTYPE *ttype = (PRT_TUPTYPE *)type.typeUnion.tuple;
		printf_s("(");
		if (ttype->arity == 1)
		{
			PrtCmdPrintType(ttype->fieldTypes[0]);
			printf_s(",)");
		}
		else
		{
			for (i = 0; i < ttype->arity; ++i)
			{
				PrtCmdPrintType(ttype->fieldTypes[i]);
				if (i < ttype->arity - 1)
				{
					printf_s(", ");
				}
				else
				{
					printf_s(")");
				}
			}
		}

		break;
	}
	default:
		PrtAssert(PRT_FALSE, "Invalid type");
		break;
	}
}

void PrtCmdPrintValue(_In_ PRT_VALUE *value)
{
	PRT_TYPE_KIND kind = value->type.typeKind;
	switch (kind)
	{
	case PRT_KIND_ANY:
		PRT_DBG_ASSERT(PRT_FALSE, "Value must have a more concrete type");
		break;
	case PRT_KIND_BOOL:
		printf_s(PrtPrimGetBool(value) == PRT_TRUE ? "true" : "false");
		break;
	case PRT_KIND_EVENT:
	{
		PRT_UINT32 event = PrtPrimGetEvent(value);
		printf_s("<%d>", event);
		break;
	}
	case PRT_KIND_NULL:
		printf_s("null");
		break;
	case PRT_KIND_MACHINE:
	{
		PRT_UINT32 machine = PrtPrimGetMachine(value);
		printf_s("<%d>", machine);

		break;
	}
	case PRT_KIND_INT:
		printf_s("%d", PrtPrimGetInt(value));
		break;
	case PRT_KIND_MODEL:
	{
		PRT_UINT32 model = PrtPrimGetModel(value);
		printf_s("<%d>", model);

		break;
	}
	case PRT_KIND_FORGN:
		printf_s("foreign");
		break;
	case PRT_KIND_MAP:
	{
		PRT_MAPVALUE *mval = value->valueUnion.map;
		PRT_MAPNODE *next = mval->first;
		printf_s("{");
		while (next != NULL)
		{
			PrtCmdPrintValue(next->key);
			printf_s(" --> ");
			PrtCmdPrintValue(next->value);
			if (next->bucketNext != NULL)
			{
				printf_s("*");
			}

			if (next->insertNext != NULL)
			{
				printf_s(", ");
			}

			next = next->insertNext;
		}

		printf_s("} (%d / %d)", mval->size, PrtMapCapacity(value));
		break;
	}
	case PRT_KIND_NMDTUP:
	{
		PRT_UINT32 i;
		PRT_TUPVALUE *tval = value->valueUnion.tuple;
		PRT_NMDTUPTYPE *ntype = value->type.typeUnion.nmTuple;
		printf_s("(");
		for (i = 0; i < ntype->arity; ++i)
		{
			printf_s("%s = ", ntype->fieldNames[i]);
			PrtCmdPrintValue(tval->values[i]);
			if (i < ntype->arity - 1)
			{
				printf_s(", ");
			}
			else
			{
				printf_s(")");
			}
		}

		break;
	}
	case PRT_KIND_SEQ:
	{
		PRT_UINT32 i;
		PRT_SEQVALUE *sVal = value->valueUnion.seq;
		printf_s("[");
		for (i = 0; i < sVal->size; ++i)
		{
			PrtCmdPrintValue(sVal->values[i]);
			if (i < sVal->size - 1)
			{
				printf_s(", ");
			}
		}

		printf_s("]");
		break;
	}
	case PRT_KIND_TUPLE:
	{
		PRT_UINT32 i;
		PRT_TUPVALUE *tval = value->valueUnion.tuple;
		PRT_TUPTYPE *ttype = value->type.typeUnion.tuple;
		printf_s("(");
		if (ttype->arity == 1)
		{
			PrtCmdPrintValue(tval->values[0]);
			printf_s(",)");
		}
		else
		{
			for (i = 0; i < ttype->arity; ++i)
			{
				PrtCmdPrintValue(tval->values[i]);
				if (i < ttype->arity - 1)
				{
					printf_s(", ");
				}
				else
				{
					printf_s(")");
				}
			}
		}

		break;
	}
	default:
		PrtAssert(PRT_FALSE, "Invalid type");
		break;
	}
}

void PrtCmdPrintValueAndType(_In_ PRT_VALUE *value)
{
	PrtCmdPrintValue(value);
	printf_s(" : ");
	PrtCmdPrintType(value->type);
}