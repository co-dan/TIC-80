#include "machine.h"

//#if defined(TIC_BUILD_WITH_GUILE)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libguile.h>

static const char TicMachine[] = "_TIC80";

// this wouldn't work with multiple guile instances
// pls hide this inside the guile interpreter
tic_mem* memory = NULL;

/* Utils */
static s32 getGuileNumber(SCM num)
{
  return (s32)scm_to_signed_integer(num, INT_MIN, INT_MAX);
}

/* Implementing the TIC-80 API */
static SCM guile_btn(SCM id)
{
  int r = 0;
  if (id != SCM_UNDEFINED)
    {
      u32 c_id = getGuileNumber(id);
      r = memory->ram.input.gamepads.data & (1 << c_id);
    }
  else
    {
      r = (int)memory->ram.input.gamepads.data;
    }
  return scm_from_bool(r);
}

static SCM guile_btnp(SCM id, SCM hold, SCM period)
{
  s32 c_id = -1;
  s32 c_hold = -1;
  s32 c_period = -1;
  
  if (id != SCM_UNDEFINED) c_id = getGuileNumber(id);
  if (hold != SCM_UNDEFINED) c_hold = getGuileNumber(hold);
  if (period != SCM_UNDEFINED) c_period = getGuileNumber(period);
  int r = (int)memory->api.btnp(memory, c_id, c_hold, c_period);
  return scm_from_bool(r);
}

static SCM guile_cls(SCM c)
{
  if(c != SCM_UNDEFINED)
    memory->api.clear(memory, getGuileNumber(c));
  else
    memory->api.clear(memory, 0);
  return SCM_UNDEFINED;
}

static SCM guile_print(SCM text, SCM x, SCM y, SCM color,
                       SCM fixed, SCM scale, SCM smallfont)
{
  if (text == SCM_UNDEFINED) return scm_from_int(0);
  char * c_txt = scm_to_stringn(text,NULL,"UTF-8",SCM_FAILED_CONVERSION_QUESTION_MARK);
  s32 c_x = 0;
  if (x != SCM_UNDEFINED) c_x = scm_to_int(x);
  s32 c_y = 0;
  if (y != SCM_UNDEFINED) c_y = scm_to_int(y);
  s32 c_color = TIC_PALETTE_SIZE-1;
  if (color != SCM_UNDEFINED) c_color = scm_to_int(color) % TIC_PALETTE_SIZE;
  bool c_fixed = false;
  if (fixed != SCM_UNDEFINED) c_fixed = scm_to_bool(fixed);
  s32 c_scale = 1;
  if (scale != SCM_UNDEFINED) c_scale = scm_to_int(scale);
  bool alt = false;
  if (smallfont != SCM_UNDEFINED) alt = scm_to_bool(smallfont);
  s32 width = memory->api.text_ex(memory, c_txt ? c_txt : "nil",
                                  c_x, c_y, c_color, c_fixed, c_scale, alt);
  return scm_from_int(width);
}

static SCM guile_rect(SCM x, SCM y, SCM w, SCM h, SCM color)
{
  s32 c_x = -1;
  s32 c_y = -1;
  s32 c_w = -1;
  s32 c_h = -1;
  s32 c_color = -1;
  if (x != SCM_UNDEFINED) c_x = getGuileNumber(x);
  if (y != SCM_UNDEFINED) c_y = getGuileNumber(y);
  if (w != SCM_UNDEFINED) c_w = getGuileNumber(w);
  if (h != SCM_UNDEFINED) c_h = getGuileNumber(h);
  if (color != SCM_UNDEFINED) c_color = getGuileNumber(color);
  memory->api.rect(memory, c_x, c_y, c_w, c_h, c_color);
  return scm_from_bool(1);
}


static void* guile_register_functions(void * data) {
  scm_c_define_gsubr("btnp", 0, 3, 0, &guile_btnp);
  scm_c_define_gsubr("cls", 0, 1, 0, &guile_cls);
  scm_c_define_gsubr("rect", 5, 0, 0, &guile_rect);
  scm_c_define_gsubr("print", 1, 6, 0, &guile_print);
  return NULL;
}

// TODO: pre-compilation
// TODO: error handling much?
static void evalGuile(tic_mem* tic, const char* code)
{
  scm_c_eval_string(code);
}

static bool initGuile(tic_mem* tic, const char* code)
{
  memory = tic;
  scm_with_guile (&guile_register_functions, NULL);
  evalGuile(tic, code);
  return true;
}

static void closeGuile(tic_mem* tic)
{
}

static void callGuileTick(tic_mem* tic)
{
  SCM ticProc = scm_c_eval_string("TIC");
  // TODO, pre-load this in init
  if (scm_procedure_p(ticProc))
    {
      // TODO: check for errors
      scm_call_0(ticProc);
    }
  else
    {
      printf("OwO not a procedure heh");
    }
}

static void callGuileScanline(tic_mem* memory, s32 row, void* data)
{
}

static void callGuileOverline(tic_mem* memory, void* data)
{
}

static const tic_outline_item* getGuileOutline(const char* code, s32* size)
{
  static tic_outline_item* items = NULL;
  
  if(items)
    {
      free(items);
      items = NULL;
    }
  
  printf("UwU..\n");
  return items;
}


static const char* const GuileKeywords [] =
{
	"do", "values", "if", "when", "each", "for", "fn", "lambda", "partial",
	"while", "set", "global", "var", "local", "let", "tset",
	"or", "and", "true", "false", "nil", "#", ":", "->", "->>"
};

static const char* const ApiKeywords[] = API_KEYWORDS;

static const tic_script_config GuileSyntaxConfig =
{
	.init 				= initGuile,
	.close 				= closeGuile,
	.tick 				= callGuileTick,
	.scanline 			= callGuileScanline,
	.overline 			= callGuileOverline,

	.getOutline			= getGuileOutline,
	.parse 				= parseCode,
	.eval				= evalGuile,

	.blockCommentStart 	= NULL,
	.blockCommentEnd 	= NULL,
	.blockStringStart	= NULL,
	.blockStringEnd		= NULL,
	.singleComment 		= ";",

	.keywords 			= GuileKeywords,
	.keywordsCount                  = COUNT_OF(GuileKeywords),

	.api 				= ApiKeywords,
	.apiCount 			= COUNT_OF(ApiKeywords),
};

const tic_script_config* getGuileConfig()
{
	return &GuileSyntaxConfig;
}



//#endif /* defined(TIC_BUILD_WITH_GUILE) */
