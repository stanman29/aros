
#define DEBUG 1
#include <aros/debug.h>

#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <libraries/mui.h>

#include "support.h"
#include "worker.h"
#include "desktop_intern.h"

#include <proto/dos.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include "presentation.h"
#include "iconcontainerclass.h"
#include "iconclass.h"

#include "desktop_intern_protos.h"

IPTR iconNew(Class *cl, Object *obj, struct opSet *msg)
{
	IPTR retval=0;
	struct IconClassData *data;
	struct TagItem *tag;
	struct DiskObject *diskobject=NULL;
	UBYTE *label=NULL;
	BOOL selected=FALSE;

	tag=FindTagItem(IA_DiskObject, msg->ops_AttrList);
	if(tag)
	{
		diskobject=tag->ti_Data;
		tag->ti_Tag=TAG_IGNORE;
	}

	tag=FindTagItem(IA_Label, msg->ops_AttrList);
	if(tag)
	{
		label=tag->ti_Data;
		tag->ti_Tag=TAG_IGNORE;
	}

	tag=FindTagItem(IA_Selected, msg->ops_AttrList);
	if(tag)
	{
		selected=tag->ti_Data;
		tag->ti_Tag=TAG_IGNORE;
	}

	retval=DoSuperMethodA(cl, obj, msg);
	if(retval)
	{
		obj=(Object*)retval;
		data=INST_DATA(cl, obj);
		data->diskObject=diskobject;
		data->label=label;
		data->selected=selected;

		data->imagePart=RectangleObject,
			ButtonFrame,
			MUIA_FixHeight, 30,
			MUIA_FixWidth, 30,
			End;

		data->labelPart=TextObject,
			MUIA_Text_Contents, label,
			End;

		DoMethod(obj, OM_ADDMEMBER, data->imagePart);
		DoMethod(obj, OM_ADDMEMBER, data->labelPart);
	}

	return retval;
}

IPTR iconSet(Class *cl, Object *obj, struct opSet *msg)
{
	struct IconClassData *data;
	IPTR retval=1;
	struct TagItem *tag, *tstate=msg->ops_AttrList;

	data=(struct IconClassData*)INST_DATA(cl, obj);

	while(tag=NextTagItem(&tstate))
    {
		switch(tag->ti_Tag)
		{
			case IA_Label:
				SetAttrs(data->labelPart, MUIA_Text_Contents, tag->ti_Data, TAG_END);
				break;
			case IA_Selected:
				data->selected=tag->ti_Data;
				SetAttrs(data->imagePart, MUIA_Selected, tag->ti_Data, TAG_END);
				if(tag->ti_Data)
					retval=DoSuperMethodA(cl, obj, (Msg)msg);
				break;
			case IA_Executed:
				retval=DoSuperMethodA(cl, obj, (Msg)msg);
				break;
			default:
				retval=DoSuperMethodA(cl, obj, (Msg)msg);
				break;
		}
	}

	return retval;
}

IPTR iconGet(Class *cl, Object *obj, struct opGet *msg)
{
	IPTR retval=1;
	struct IconClassData *data;

	data=(struct IconClassData*)INST_DATA(cl, obj);

	switch(msg->opg_AttrID)
	{
		default:
			retval=DoSuperMethodA(cl, obj, (Msg)msg);
			break;
	}

	return retval;
}

IPTR iconDispose(Class *cl, Object *obj, Msg msg)
{
	IPTR retval;
	struct IconClassData *data;

	data=(struct IconClassData*)INST_DATA(cl, obj);

	retval=DoSuperMethodA(cl, obj, msg);

	return retval;
}

#define MAX(x, y) (x>y?x:y)

IPTR iconAskMinMax(Class *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
	IPTR retval;
	struct MUI_MinMax minMax;
	struct IconClassData *data;

	data=(struct IconClassData*)INST_DATA(cl, obj);

	retval=DoSuperMethodA(cl, obj, (Msg)msg);

	msg->MinMaxInfo->MinWidth+=(MAX(_minwidth(data->imagePart),_minwidth(data->labelPart)));
	msg->MinMaxInfo->DefWidth+=(MAX(_minwidth(data->imagePart),_minwidth(data->labelPart)));
	msg->MinMaxInfo->MaxWidth+=(MAX(_minwidth(data->imagePart),_minwidth(data->labelPart)));

	msg->MinMaxInfo->MinHeight+=(_minheight(data->imagePart)+_minheight(data->labelPart));
	msg->MinMaxInfo->DefHeight+=(_minheight(data->imagePart)+_minheight(data->labelPart));
	msg->MinMaxInfo->MaxHeight+=(_minheight(data->imagePart)+_minheight(data->labelPart));

	return retval;
}

IPTR iconHandleInput(Class *cl, Object *obj, struct MUIP_HandleInput *msg)
{
	IPTR retval=0;
	struct IconClassData *data;

	data=(struct IconClassData*)INST_DATA(cl, obj);

	if(msg->imsg)
	{
		switch(msg->imsg->Class)
		{
			case IDCMP_MOUSEBUTTONS:
			{
				if(msg->imsg->Code==SELECTDOWN)
				{
					if(msg->imsg->MouseX >= _mleft(obj) && msg->imsg->MouseX <= _mright(obj) &&
						msg->imsg->MouseY >= _mtop(obj) && msg->imsg->MouseY <= _mbottom(obj))
					{
						if(data->selected)
						{
							ULONG nowSeconds=0, nowMicros=0;

							CurrentTime(&nowSeconds, &nowMicros);
							DoubleClick(data->lastClickSecs, data->lastClickMicros, nowSeconds, nowMicros);
							SetAttrs(obj, IA_Executed, TRUE, TAG_END);
						}
						else
						{
							CurrentTime(&data->lastClickSecs, &data->lastClickMicros);
							DoMethod(_parent(obj), ICM_UnselectAll, obj);
							SetAttrs(obj, IA_Selected, TRUE, TAG_END);
						}
					}

				}
				break;
			}
		}
	}
}

IPTR iconSetup(Class *cl, Object *obj, struct MUIP_Setup *msg)
{
	IPTR retval=0;

	DoSuperMethodA(cl, obj, msg);

	MUI_RequestIDCMP(obj, IDCMP_MOUSEBUTTONS);

	return retval;
}

AROS_UFH3(IPTR, iconDispatcher,
	AROS_UFHA(Class  *, cl,  A0),
	AROS_UFHA(Object *, obj, A2),
	AROS_UFHA(Msg     , msg, A1))
{
	ULONG retval=0;

	switch(msg->MethodID)
	{
		case OM_NEW:
			retval=iconNew(cl, obj, (struct opSet*)msg);
			break;
		case OM_SET:
			retval=iconSet(cl, obj, (struct opSet*)msg);
			break;
		case OM_GET:
			retval=iconGet(cl, obj, (struct opGet*)msg);
			break;
		case OM_DISPOSE:
			retval=iconDispose(cl, obj, msg);
			break;
//		case MUIM_AskMinMax:
//			retval=iconAskMinMax(cl, obj, (struct MUIP_AskMinMax*)msg);
//			break;
		case MUIM_HandleInput:
			retval=iconHandleInput(cl, obj, (struct MUIP_HandleInput*)msg);
			break;
		case MUIM_Setup:
			retval=iconSetup(cl, obj, (struct MUIP_Setup*)msg);
			break;
		default:
			retval=DoSuperMethodA(cl, obj, msg);
			break;
	}

	return retval;
}

