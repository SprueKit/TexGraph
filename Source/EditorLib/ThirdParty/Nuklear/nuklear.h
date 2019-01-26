/*
Nuklear - 1.37.0 - public domain
no warrenty implied; use at your own risk.
authored from 2015-2017 by Micha Mettke

ABOUT:
This is a minimal state graphical user interface single header toolkit
written in ANSI C and licensed under public domain.
It was designed as a simple embeddable user interface for application and does
not have any dependencies, a default renderbackend or OS window and input handling
but instead provides a very modular library approach by using simple input state
for input and draw commands describing primitive shapes as output.
So instead of providing a layered library that tries to abstract over a number
of platform and render backends it only focuses on the actual UI.

VALUES:
- Graphical user interface toolkit
- Single header library
- Written in C89 (a.k.a. ANSI C or ISO C90)
- Small codebase (~17kLOC)
- Focus on portability, efficiency and simplicity
- No dependencies (not even the standard library if not wanted)
- Fully skinnable and customizable
- Low memory footprint with total memory control if needed or wanted
- UTF-8 support
- No global or hidden state
- Customizable library modules (you can compile and use only what you need)
- Optional font baker and vertex buffer output

USAGE:
This library is self contained in one single header file and can be used either
in header only mode or in implementation mode. The header only mode is used
by default when included and allows including this header in other headers
and does not contain the actual implementation.

The implementation mode requires to define  the preprocessor macro
NK_IMPLEMENTATION in *one* .c/.cpp file before #includeing this file, e.g.:

#define NK_IMPLEMENTATION
#include "nuklear.h"

Also optionally define the symbols listed in the section "OPTIONAL DEFINES"
below in header and implementation mode if you want to use additional functionality
or need more control over the library.
IMPORTANT:  Every time you include "nuklear.h" you have to define the same flags.
This is very important not doing it either leads to compiler errors
or even worse stack corruptions.

FEATURES:
- Absolutely no platform dependend code
- Memory management control ranging from/to
- Ease of use by allocating everything from standard library
- Control every byte of memory inside the library
- Font handling control ranging from/to
- Use your own font implementation for everything
- Use this libraries internal font baking and handling API
- Drawing output control ranging from/to
- Simple shapes for more high level APIs which already have drawing capabilities
- Hardware accessible anti-aliased vertex buffer output
- Customizable colors and properties ranging from/to
- Simple changes to color by filling a simple color table
- Complete control with ability to use skinning to decorate widgets
- Bendable UI library with widget ranging from/to
- Basic widgets like buttons, checkboxes, slider, ...
- Advanced widget like abstract comboboxes, contextual menus,...
- Compile time configuration to only compile what you need
- Subset which can be used if you do not want to link or use the standard library
- Can be easily modified to only update on user input instead of frame updates

OPTIONAL DEFINES:
NK_PRIVATE
If defined declares all functions as static, so they can only be accessed
inside the file that contains the implementation

NK_INCLUDE_FIXED_TYPES
If defined it will include header <stdint.h> for fixed sized types
otherwise nuklear tries to select the correct type. If that fails it will
throw a compiler error and you have to select the correct types yourself.
<!> If used needs to be defined for implementation and header <!>

NK_INCLUDE_DEFAULT_ALLOCATOR
if defined it will include header <stdlib.h> and provide additional functions
to use this library without caring for memory allocation control and therefore
ease memory management.
<!> Adds the standard library with malloc and free so don't define if you
don't want to link to the standard library <!>
<!> If used needs to be defined for implementation and header <!>

NK_INCLUDE_STANDARD_IO
if defined it will include header <stdio.h> and provide
additional functions depending on file loading.
<!> Adds the standard library with fopen, fclose,... so don't define this
if you don't want to link to the standard library <!>
<!> If used needs to be defined for implementation and header <!>

NK_INCLUDE_STANDARD_VARARGS
if defined it will include header <stdarg.h> and provide
additional functions depending on variable arguments
<!> Adds the standard library with va_list and  so don't define this if
you don't want to link to the standard library<!>
<!> If used needs to be defined for implementation and header <!>

NK_INCLUDE_VERTEX_BUFFER_OUTPUT
Defining this adds a vertex draw command list backend to this
library, which allows you to convert queue commands into vertex draw commands.
This is mainly if you need a hardware accessible format for OpenGL, DirectX,
Vulkan, Metal,...
<!> If used needs to be defined for implementation and header <!>

NK_INCLUDE_FONT_BAKING
Defining this adds the `stb_truetype` and `stb_rect_pack` implementation
to this library and provides font baking and rendering.
If you already have font handling or do not want to use this font handler
you don't have to define it.
<!> If used needs to be defined for implementation and header <!>

NK_INCLUDE_DEFAULT_FONT
Defining this adds the default font: ProggyClean.ttf into this library
which can be loaded into a font atlas and allows using this library without
having a truetype font
<!> Enabling this adds ~12kb to global stack memory <!>
<!> If used needs to be defined for implementation and header <!>

NK_INCLUDE_COMMAND_USERDATA
Defining this adds a userdata pointer into each command. Can be useful for
example if you want to provide custom shaders depending on the used widget.
Can be combined with the style structures.
<!> If used needs to be defined for implementation and header <!>

NK_BUTTON_TRIGGER_ON_RELEASE
Different platforms require button clicks occuring either on buttons being
pressed (up to down) or released (down to up).
By default this library will react on buttons being pressed, but if you
define this it will only trigger if a button is released.
<!> If used it is only required to be defined for the implementation part <!>

NK_ZERO_COMMAND_MEMORY
Defining this will zero out memory for each drawing command added to a
drawing queue (inside nk_command_buffer_push). Zeroing command memory
is very useful for fast checking (using memcmp) if command buffers are
equal and avoid drawing frames when nothing on screen has changed since
previous frame.

NK_ASSERT
If you don't define this, nuklear will use <assert.h> with assert().
<!> Adds the standard library so define to nothing of not wanted <!>
<!> If used needs to be defined for implementation and header <!>

NK_BUFFER_DEFAULT_INITIAL_SIZE
Initial buffer size allocated by all buffers while using the default allocator
functions included by defining NK_INCLUDE_DEFAULT_ALLOCATOR. If you don't
want to allocate the default 4k memory then redefine it.
<!> If used needs to be defined for implementation and header <!>

NK_MAX_NUMBER_BUFFER
Maximum buffer size for the conversion buffer between float and string
Under normal circumstances this should be more than sufficient.
<!> If used needs to be defined for implementation and header <!>

NK_INPUT_MAX
Defines the max number of bytes which can be added as text input in one frame.
Under normal circumstances this should be more than sufficient.
<!> If used it is only required to be defined for the implementation part <!>

NK_MEMSET
You can define this to 'memset' or your own memset implementation
replacement. If not nuklear will use its own version.
<!> If used it is only required to be defined for the implementation part <!>

NK_MEMCPY
You can define this to 'memcpy' or your own memcpy implementation
replacement. If not nuklear will use its own version.
<!> If used it is only required to be defined for the implementation part <!>

NK_SQRT
You can define this to 'sqrt' or your own sqrt implementation
replacement. If not nuklear will use its own slow and not highly
accurate version.
<!> If used it is only required to be defined for the implementation part <!>

NK_SIN
You can define this to 'sinf' or your own sine implementation
replacement. If not nuklear will use its own approximation implementation.
<!> If used it is only required to be defined for the implementation part <!>

NK_COS
You can define this to 'cosf' or your own cosine implementation
replacement. If not nuklear will use its own approximation implementation.
<!> If used it is only required to be defined for the implementation part <!>

NK_STRTOD
You can define this to `strtod` or your own string to double conversion
implementation replacement. If not defined nuklear will use its own
imprecise and possibly unsafe version (does not handle nan or infinity!).
<!> If used it is only required to be defined for the implementation part <!>

NK_DTOA
You can define this to `dtoa` or your own double to string conversion
implementation replacement. If not defined nuklear will use its own
imprecise and possibly unsafe version (does not handle nan or infinity!).
<!> If used it is only required to be defined for the implementation part <!>

NK_VSNPRINTF
If you define `NK_INCLUDE_STANDARD_VARARGS` as well as `NK_INCLUDE_STANDARD_IO`
and want to be safe define this to `vsnprintf` on compilers supporting
later versions of C or C++. By default nuklear will check for your stdlib version
in C as well as compiler version in C++. if `vsnprintf` is available
it will define it to `vsnprintf` directly. If not defined and if you have
older versions of C or C++ it will be defined to `vsprintf` which is unsafe.
<!> If used it is only required to be defined for the implementation part <!>

NK_BYTE
NK_INT16
NK_UINT16
NK_INT32
NK_UINT32
NK_SIZE_TYPE
NK_POINTER_TYPE
If you compile without NK_USE_FIXED_TYPE then a number of standard types
will be selected and compile time validated. If they are incorrect you can
define the correct types by overloading these type defines.

CREDITS:
Developed by Micha Mettke and every direct or indirect contributor.

Embeds stb_texedit, stb_truetype and stb_rectpack by Sean Barret (public domain)
Embeds ProggyClean.ttf font by Tristan Grimmer (MIT license).

Big thank you to Omar Cornut (ocornut@github) for his imgui library and
giving me the inspiration for this library, Casey Muratori for handmade hero
and his original immediate mode graphical user interface idea and Sean
Barret for his amazing single header libraries which restored my faith
in libraries and brought me to create some of my own.

LICENSE:
This software is dual-licensed to the public domain and under the following
license: you are granted a perpetual, irrevocable license to copy, modify,
publish and distribute this file as you see fit.
*/
#ifndef NK_NUKLEAR_H_
#define NK_NUKLEAR_H_

#ifdef __cplusplus
extern "C" {
#endif
    /*
    * ==============================================================
    *
    *                          CONSTANTS
    *
    * ===============================================================
    */
#define NK_UNDEFINED (-1.0f)
#define NK_UTF_INVALID 0xFFFD /* internal invalid utf8 rune */
#define NK_UTF_SIZE 4 /* describes the number of bytes a glyph consists of*/
#ifndef NK_INPUT_MAX
#define NK_INPUT_MAX 16
#endif
#ifndef NK_MAX_NUMBER_BUFFER
#define NK_MAX_NUMBER_BUFFER 64
#endif
#ifndef NK_SCROLLBAR_HIDING_TIMEOUT
#define NK_SCROLLBAR_HIDING_TIMEOUT 4.0f
#endif
    /*
    * ==============================================================
    *
    *                          HELPER
    *
    * ===============================================================
    */
#ifndef NK_API
#ifdef NK_PRIVATE
#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199409L))
#define NK_API static inline
#elif defined(__cplusplus)
#define NK_API static inline
#else
#define NK_API static
#endif
#else
#define NK_API extern
#endif
#endif

#define NK_INTERN static
#define NK_STORAGE static
#define NK_GLOBAL static

#define NK_FLAG(x) (1 << (x))
#define NK_STRINGIFY(x) #x
#define NK_MACRO_STRINGIFY(x) NK_STRINGIFY(x)
#define NK_STRING_JOIN_IMMEDIATE(arg1, arg2) arg1 ## arg2
#define NK_STRING_JOIN_DELAY(arg1, arg2) NK_STRING_JOIN_IMMEDIATE(arg1, arg2)
#define NK_STRING_JOIN(arg1, arg2) NK_STRING_JOIN_DELAY(arg1, arg2)

#ifdef _MSC_VER
#define NK_UNIQUE_NAME(name) NK_STRING_JOIN(name,__COUNTER__)
#else
#define NK_UNIQUE_NAME(name) NK_STRING_JOIN(name,__LINE__)
#endif

#ifndef NK_STATIC_ASSERT
#define NK_STATIC_ASSERT(exp) typedef char NK_UNIQUE_NAME(_dummy_array)[(exp)?1:-1]
#endif

#ifndef NK_FILE_LINE
#ifdef _MSC_VER
#define NK_FILE_LINE __FILE__ ":" NK_MACRO_STRINGIFY(__COUNTER__)
#else
#define NK_FILE_LINE __FILE__ ":" NK_MACRO_STRINGIFY(__LINE__)
#endif
#endif

#define NK_MIN(a,b) ((a) < (b) ? (a) : (b))
#define NK_MAX(a,b) ((a) < (b) ? (b) : (a))
#define NK_CLAMP(i,v,x) (NK_MAX(NK_MIN(v,x), i))
    /*
    * ===============================================================
    *
    *                          BASIC
    *
    * ===============================================================
    */
#ifdef NK_INCLUDE_FIXED_TYPES
#include <stdint.h>
#define NK_INT8 int8_t
#define NK_UINT8 uint8_t
#define NK_INT16 int16_t
#define NK_UINT16 uint16_t
#define NK_INT32 int32_t
#define NK_UINT32 uint32_t
#define NK_SIZE_TYPE uintptr_t
#define NK_POINTER_TYPE uintptr_t
#else
#ifndef NK_INT8
#define NK_INT8 char
#endif
#ifndef NK_UINT8
#define NK_UINT8 unsigned char
#endif
#ifndef NK_INT16
#define NK_INT16 signed short
#endif
#ifndef NK_UINT16
#define NK_UINT16 unsigned short
#endif
#ifndef NK_INT32
#if defined(_MSC_VER)
#define NK_INT32 __int32
#else
#define NK_INT32 signed int
#endif
#endif
#ifndef NK_UINT32
#if defined(_MSC_VER)
#define NK_UINT32 unsigned __int32
#else
#define NK_UINT32 unsigned int
#endif
#endif
#ifndef NK_SIZE_TYPE
#if defined(_WIN64) && defined(_MSC_VER)
#define NK_SIZE_TYPE unsigned __int64
#elif (defined(_WIN32) || defined(WIN32)) && defined(_MSC_VER)
#define NK_SIZE_TYPE unsigned __int32
#elif defined(__GNUC__) || defined(__clang__)
#if defined(__x86_64__) || defined(__ppc64__)
#define NK_SIZE_TYPE unsigned long
#else
#define NK_SIZE_TYPE unsigned int
#endif
#else
#define NK_SIZE_TYPE unsigned long
#endif
#endif
#ifndef NK_POINTER_TYPE
#if defined(_WIN64) && defined(_MSC_VER)
#define NK_POINTER_TYPE unsigned __int64
#elif (defined(_WIN32) || defined(WIN32)) && defined(_MSC_VER)
#define NK_POINTER_TYPE unsigned __int32
#elif defined(__GNUC__) || defined(__clang__)
#if defined(__x86_64__) || defined(__ppc64__)
#define NK_POINTER_TYPE unsigned long
#else
#define NK_POINTER_TYPE unsigned int
#endif
#else
#define NK_POINTER_TYPE unsigned long
#endif
#endif
#endif

    typedef NK_INT8 nk_char;
    typedef NK_UINT8 nk_uchar;
    typedef NK_UINT8 nk_byte;
    typedef NK_INT16 nk_short;
    typedef NK_UINT16 nk_ushort;
    typedef NK_INT32 nk_int;
    typedef NK_UINT32 nk_uint;
    typedef NK_SIZE_TYPE nk_size;
    typedef NK_POINTER_TYPE nk_ptr;

    typedef nk_uint nk_hash;
    typedef nk_uint nk_flags;
    typedef nk_uint nk_rune;

    /* Make sure correct type size:
    * This will fire with a negative subscript error if the type sizes
    * are set incorrectly by the compiler, and compile out if not */
    NK_STATIC_ASSERT(sizeof(nk_short) == 2);
    NK_STATIC_ASSERT(sizeof(nk_ushort) == 2);
    NK_STATIC_ASSERT(sizeof(nk_uint) == 4);
    NK_STATIC_ASSERT(sizeof(nk_int) == 4);
    NK_STATIC_ASSERT(sizeof(nk_byte) == 1);
    NK_STATIC_ASSERT(sizeof(nk_flags) >= 4);
    NK_STATIC_ASSERT(sizeof(nk_rune) >= 4);
    NK_STATIC_ASSERT(sizeof(nk_size) >= sizeof(void*));
    NK_STATIC_ASSERT(sizeof(nk_ptr) >= sizeof(void*));

    /* ============================================================================
    *
    *                                  API
    *
    * =========================================================================== */
    struct nk_buffer;
    struct nk_allocator;
    struct nk_command_buffer;
    struct nk_draw_command;
    struct nk_convert_config;
    struct nk_style_item;
    struct nk_text_edit;
    struct nk_draw_list;
    struct nk_user_font;
    struct nk_panel;
    struct nk_context;
    struct nk_draw_vertex_layout_element;
    struct nk_style_button;
    struct nk_style_toggle;
    struct nk_style_selectable;
    struct nk_style_slide;
    struct nk_style_progress;
    struct nk_style_scrollbar;
    struct nk_style_edit;
    struct nk_style_property;
    struct nk_style_chart;
    struct nk_style_combo;
    struct nk_style_tab;
    struct nk_style_window_header;
    struct nk_style_window;

    enum { nk_false, nk_true };
    struct nk_color { nk_byte r, g, b, a; };
    struct nk_colorf { float r, g, b, a; };
    struct nk_vec2 { float x, y; };
    struct nk_vec2i { short x, y; };
    struct nk_rect { float x, y, w, h; };
    struct nk_recti { short x, y, w, h; };
    typedef char nk_glyph[NK_UTF_SIZE];
    typedef union { void *ptr; int id; } nk_handle;
    struct nk_image { nk_handle handle; unsigned short w, h; unsigned short region[4]; };
    struct nk_cursor { struct nk_image img; struct nk_vec2 size, offset; };
    struct nk_scroll { nk_uint x, y; };

    enum nk_heading { NK_UP, NK_RIGHT, NK_DOWN, NK_LEFT };
    enum nk_button_behavior { NK_BUTTON_DEFAULT, NK_BUTTON_REPEATER };
    enum nk_modify { NK_FIXED = nk_false, NK_MODIFIABLE = nk_true };
    enum nk_orientation { NK_VERTICAL, NK_HORIZONTAL };
    enum nk_collapse_states { NK_MINIMIZED = nk_false, NK_MAXIMIZED = nk_true };
    enum nk_show_states { NK_HIDDEN = nk_false, NK_SHOWN = nk_true };
    enum nk_chart_type { NK_CHART_LINES, NK_CHART_COLUMN, NK_CHART_MAX };
    enum nk_chart_event { NK_CHART_HOVERING = 0x01, NK_CHART_CLICKED = 0x02 };
    enum nk_color_format { NK_RGB, NK_RGBA };
    enum nk_popup_type { NK_POPUP_STATIC, NK_POPUP_DYNAMIC };
    enum nk_layout_format { NK_DYNAMIC, NK_STATIC };
    enum nk_tree_type { NK_TREE_NODE, NK_TREE_TAB };
    /// JSandusky
    enum nk_layout_valign { NK_LAYOUT_TOP, NK_LAYOUT_MIDDLE, NK_LAYOUT_BOTTOM };
    enum nk_layout_halign { NK_LAYOUT_LEFT, NK_LAYOUT_CENTER, NK_LAYOUT_RIGHT };

    typedef void*(*nk_plugin_alloc)(nk_handle, void *old, nk_size);
    typedef void(*nk_plugin_free)(nk_handle, void *old);
    typedef int(*nk_plugin_filter)(const struct nk_text_edit*, nk_rune unicode);
    typedef void(*nk_plugin_paste)(nk_handle, struct nk_text_edit*);
    typedef void(*nk_plugin_copy)(nk_handle, const char*, int len);

    struct nk_allocator {
        nk_handle userdata;
        nk_plugin_alloc alloc;
        nk_plugin_free free;
    };
    enum nk_symbol_type {
        NK_SYMBOL_NONE,
        NK_SYMBOL_X,
        NK_SYMBOL_UNDERSCORE,
        NK_SYMBOL_CIRCLE_SOLID,
        NK_SYMBOL_CIRCLE_OUTLINE,
        NK_SYMBOL_RECT_SOLID,
        NK_SYMBOL_RECT_OUTLINE,
        NK_SYMBOL_TRIANGLE_UP,
        NK_SYMBOL_TRIANGLE_DOWN,
        NK_SYMBOL_TRIANGLE_LEFT,
        NK_SYMBOL_TRIANGLE_RIGHT,
        NK_SYMBOL_PLUS,
        NK_SYMBOL_MINUS,
        NK_SYMBOL_MAX
    };
    /* =============================================================================
    *
    *                                  CONTEXT
    *
    * =============================================================================*/
    /*  Contexts are the main entry point and the majestro of nuklear and contain all required state.
    *  They are used for window, memory, input, style, stack, commands and time management and need
    *  to be passed into all nuklear GUI specific functions.
    *
    *  Usage
    *  -------------------
    *  To use a context it first has to be initialized which can be achieved by calling
    *  one of either `nk_init_default`, `nk_init_fixed`, `nk_init`, `nk_init_custom`.
    *  Each takes in a font handle and a specific way of handling memory. Memory control
    *  hereby ranges from standard library to just specifing a fixed sized block of memory
    *  which nuklear has to manage itself from.
    *
    *      struct nk_context ctx;
    *      nk_init_xxx(&ctx, ...);
    *      while (1) {
    *          [...]
    *          nk_clear(&ctx);
    *      }
    *      nk_free(&ctx);
    *
    *  Reference
    *  -------------------
    *  nk_init_default     - Initializes context with standard library memory alloction (malloc,free)
    *  nk_init_fixed       - Initializes context from single fixed size memory block
    *  nk_init             - Initializes context with memory allocator callbacks for alloc and free
    *  nk_init_custom      - Initializes context from two buffers. One for draw commands the other for window/panel/table allocations
    *  nk_clear            - Called at the end of the frame to reset and prepare the context for the next frame
    *  nk_free             - Shutdown and free all memory allocated inside the context
    *  nk_set_user_data    - Utility function to pass user data to draw command
    */
#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
    /*  nk_init_default - Initializes a `nk_context` struct with a default standard library allocator.
    *  Should be used if you don't want to be bothered with memory management in nuklear.
    *  Parameters:
    *      @ctx must point to an either stack or heap allocated `nk_context` struct
    *      @font must point to a previously initialized font handle for more info look at font documentation
    *  Return values:
    *      true(1) on success
    *      false(0) on failure */
    NK_API int nk_init_default(struct nk_context*, const struct nk_user_font*);
#endif
    /*  nk_init_fixed - Initializes a `nk_context` struct from a single fixed size memory block
    *  Should be used if you want complete control over nuklears memory management.
    *  Especially recommended for system with little memory or systems with virtual memory.
    *  For the later case you can just allocate for example 16MB of virtual memory
    *  and only the required amount of memory will actually be commited.
    *  IMPORTANT: make sure the passed memory block is aligned correctly for `nk_draw_commands`
    *  Parameters:
    *      @ctx must point to an either stack or heap allocated `nk_context` struct
    *      @memory must point to a previously allocated memory block
    *      @size must contain the total size of @memory
    *      @font must point to a previously initialized font handle for more info look at font documentation
    *  Return values:
    *      true(1) on success
    *      false(0) on failure */
    NK_API int nk_init_fixed(struct nk_context*, void *memory, nk_size size, const struct nk_user_font*);
    /*  nk_init - Initializes a `nk_context` struct with memory allocation callbacks for nuklear to allocate
    *  memory from. Used internally for `nk_init_default` and provides a kitchen sink allocation
    *  interface to nuklear. Can be useful for cases like monitoring memory consumption.
    *  Parameters:
    *      @ctx must point to an either stack or heap allocated `nk_context` struct
    *      @alloc must point to a previously allocated memory allocator
    *      @font must point to a previously initialized font handle for more info look at font documentation
    *  Return values:
    *      true(1) on success
    *      false(0) on failure */
    NK_API int nk_init(struct nk_context*, struct nk_allocator*, const struct nk_user_font*);
    /*  nk_init_custom - Initializes a `nk_context` struct from two different either fixed or growing
    *  buffers. The first buffer is for allocating draw commands while the second buffer is
    *  used for allocating windows, panels and state tables.
    *  Parameters:
    *      @ctx must point to an either stack or heap allocated `nk_context` struct
    *      @cmds must point to a previously initialized memory buffer either fixed or dynamic to store draw commands into
    *      @pool must point to a previously initialized memory buffer either fixed or dynamic to store windows, panels and tables
    *      @font must point to a previously initialized font handle for more info look at font documentation
    *  Return values:
    *      true(1) on success
    *      false(0) on failure */
    NK_API int nk_init_custom(struct nk_context*, struct nk_buffer *cmds, struct nk_buffer *pool, const struct nk_user_font*);
    /*  nk_clear - Resets the context state at the end of the frame. This includes mostly
    *  garbage collector tasks like removing windows or table not called and therefore
    *  used anymore.
    *  Parameters:
    *      @ctx must point to a previously initialized `nk_context` struct */
    NK_API void nk_clear(struct nk_context*);
    /*  nk_free - Frees all memory allocated by nuklear. Not needed if context was
    *  initialized with `nk_init_fixed`.
    *  Parameters:
    *      @ctx must point to a previously initialized `nk_context` struct */
    NK_API void nk_free(struct nk_context*);
#ifdef NK_INCLUDE_COMMAND_USERDATA
    /*  nk_set_user_data - Sets the currently passed userdata passed down into each draw command.
    *  Parameters:
    *      @ctx must point to a previously initialized `nk_context` struct
    *      @data handle with either pointer or index to be passed into every draw commands */
    NK_API void nk_set_user_data(struct nk_context*, nk_handle handle);
#endif
    /* =============================================================================
    *
    *                                  INPUT
    *
    * =============================================================================*/
    /*  The input API is responsible for holding the current input state composed of
    *  mouse, key and text input states.
    *  It is worth noting that no direct os or window handling is done in nuklear.
    *  Instead all input state has to be provided by platform specific code. This in one hand
    *  expects more work from the user and complicates usage but on the other hand
    *  provides simple abstraction over a big number of platforms, libraries and other
    *  already provided functionality.
    *
    *  Usage
    *  -------------------
    *  Input state needs to be provided to nuklear by first calling `nk_input_begin`
    *  which resets internal state like delta mouse position and button transistions.
    *  After `nk_input_begin` all current input state needs to be provided. This includes
    *  mouse motion, button and key pressed and released, text input and scrolling.
    *  Both event- or state-based input handling are supported by this API
    *  and should work without problems. Finally after all input state has been
    *  mirrored `nk_input_end` needs to be called to finish input process.
    *
    *      struct nk_context ctx;
    *      nk_init_xxx(&ctx, ...);
    *      while (1) {
    *          Event evt;
    *          nk_input_begin(&ctx);
    *          while (GetEvent(&evt)) {
    *              if (evt.type == MOUSE_MOVE)
    *                  nk_input_motion(&ctx, evt.motion.x, evt.motion.y);
    *              else if (evt.type == ...) {
    *                  ...
    *              }
    *          }
    *          nk_input_end(&ctx);
    *          [...]
    *          nk_clear(&ctx);
    *      }
    *      nk_free(&ctx);
    *
    *  Reference
    *  -------------------
    *  nk_input_begin      - Begins the input mirroring process. Needs to be called before all other `nk_input_xxx` calls
    *  nk_input_motion     - Mirrors mouse cursor position
    *  nk_input_key        - Mirrors key state with either pressed or released
    *  nk_input_button     - Mirrors mouse button state with either pressed or released
    *  nk_input_scroll     - Mirrors mouse scroll values
    *  nk_input_char       - Adds a single ASCII text character into an internal text buffer
    *  nk_input_glyph      - Adds a single multi-byte UTF-8 character into an internal text buffer
    *  nk_input_unicode    - Adds a single unicode rune into an internal text buffer
    *  nk_input_end        - Ends the input mirroring process by calculating state changes. Don't call any `nk_input_xxx` function referenced above after this call
    */
    enum nk_keys {
        NK_KEY_NONE,
        NK_KEY_SHIFT,
        NK_KEY_CTRL,
        NK_KEY_DEL,
        NK_KEY_ENTER,
        NK_KEY_TAB,
        NK_KEY_BACKSPACE,
        NK_KEY_COPY,
        NK_KEY_CUT,
        NK_KEY_PASTE,
        NK_KEY_UP,
        NK_KEY_DOWN,
        NK_KEY_LEFT,
        NK_KEY_RIGHT,
        /* Shortcuts: text field */
        NK_KEY_TEXT_INSERT_MODE,
        NK_KEY_TEXT_REPLACE_MODE,
        NK_KEY_TEXT_RESET_MODE,
        NK_KEY_TEXT_LINE_START,
        NK_KEY_TEXT_LINE_END,
        NK_KEY_TEXT_START,
        NK_KEY_TEXT_END,
        NK_KEY_TEXT_UNDO,
        NK_KEY_TEXT_REDO,
        NK_KEY_TEXT_SELECT_ALL,
        NK_KEY_TEXT_WORD_LEFT,
        NK_KEY_TEXT_WORD_RIGHT,
        /* Shortcuts: scrollbar */
        NK_KEY_SCROLL_START,
        NK_KEY_SCROLL_END,
        NK_KEY_SCROLL_DOWN,
        NK_KEY_SCROLL_UP,
        NK_KEY_MAX
    };
    enum nk_buttons {
        NK_BUTTON_LEFT,
        NK_BUTTON_MIDDLE,
        NK_BUTTON_RIGHT,
        NK_BUTTON_DOUBLE,
        NK_BUTTON_MAX
    };
    /*  nk_input_begin - Begins the input mirroring process by resetting text, scroll
    *  mouse previous mouse position and movement as well as key state transistions,
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct */
    NK_API void nk_input_begin(struct nk_context*);
    /*  nk_input_motion - Mirros current mouse position to nuklear
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @x must constain an integer describing the current mouse cursor x-position
    *      @y must constain an integer describing the current mouse cursor y-position */
    NK_API void nk_input_motion(struct nk_context*, int x, int y);
    /*  nk_input_key - Mirros state of a specific key to nuklear
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @key must be any value specified in enum `nk_keys` that needs to be mirrored
    *      @down must be 0 for key is up and 1 for key is down */
    NK_API void nk_input_key(struct nk_context*, enum nk_keys, int down);
    /*  nk_input_button - Mirros the state of a specific mouse button to nuklear
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @nk_buttons must be any value specified in enum `nk_buttons` that needs to be mirrored
    *      @x must constain an integer describing mouse cursor x-position on click up/down
    *      @y must constain an integer describing mouse cursor y-position on click up/down
    *      @down must be 0 for key is up and 1 for key is down */
    NK_API void nk_input_button(struct nk_context*, enum nk_buttons, int x, int y, int down);
    /*  nk_input_char - Copies a single ASCII character into an internal text buffer
    *  This is basically a helper function to quickly push ASCII characters into
    *  nuklear. Note that you can only push up to NK_INPUT_MAX bytes into
    *  struct `nk_input` between `nk_input_begin` and `nk_input_end`.
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @c must be a single ASCII character preferable one that can be printed */
    NK_API void nk_input_scroll(struct nk_context*, struct nk_vec2 val);
    /*  nk_input_char - Copies a single ASCII character into an internal text buffer
    *  This is basically a helper function to quickly push ASCII characters into
    *  nuklear. Note that you can only push up to NK_INPUT_MAX bytes into
    *  struct `nk_input` between `nk_input_begin` and `nk_input_end`.
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @c must be a single ASCII character preferable one that can be printed */
    NK_API void nk_input_char(struct nk_context*, char);
    /*  nk_input_unicode - Converts a encoded unicode rune into UTF-8 and copies the result
    *  into an internal text buffer.
    *  Note that you can only push up to NK_INPUT_MAX bytes into
    *  struct `nk_input` between `nk_input_begin` and `nk_input_end`.
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @glyph UTF-32 uncode codepoint */
    NK_API void nk_input_glyph(struct nk_context*, const nk_glyph);
    /*  nk_input_unicode - Converts a unicode rune into UTF-8 and copies the result
    *  into an internal text buffer.
    *  Note that you can only push up to NK_INPUT_MAX bytes into
    *  struct `nk_input` between `nk_input_begin` and `nk_input_end`.
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @glyph UTF-32 uncode codepoint */
    NK_API void nk_input_unicode(struct nk_context*, nk_rune);
    /*  nk_input_end - End the input mirroring process by resetting mouse grabbing
    *  state to ensure the mouse cursor is not grabbed indefinitely.
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct */
    NK_API void nk_input_end(struct nk_context*);
    /* =============================================================================
    *
    *                                  DRAWING
    *
    * =============================================================================*/
    /*  This library was designed to be render backend agnostic so it does
    *  not draw anything to screen directly. Instead all drawn shapes, widgets
    *  are made of, are buffered into memory and make up a command queue.
    *  Each frame therefore fills the command buffer with draw commands
    *  that then need to be executed by the user and his own render backend.
    *  After that the command buffer needs to be cleared and a new frame can be
    *  started. It is probably important to note that the command buffer is the main
    *  drawing API and the optional vertex buffer API only takes this format and
    *  converts it into a hardware accessible format.
    *
    *  Usage
    *  -------------------
    *  To draw all draw commands accumulated over a frame you need your own render
    *  backend able to draw a number of 2D primitives. This includes at least
    *  filled and stroked rectangles, circles, text, lines, triangles and scissors.
    *  As soon as this criterion is met you can iterate over each draw command
    *  and execute each draw command in a interpreter like fashion:
    *
    *      const struct nk_command *cmd = 0;
    *      nk_foreach(cmd, &ctx) {
    *      switch (cmd->type) {
    *      case NK_COMMAND_LINE:
    *          your_draw_line_function(...)
    *          break;
    *      case NK_COMMAND_RECT
    *          your_draw_rect_function(...)
    *          break;
    *      case ...:
    *          [...]
    *      }
    *
    *  In program flow context draw commands need to be executed after input has been
    *  gathered and the complete UI with windows and their contained widgets have
    *  been executed and before calling `nk_clear` which frees all previously
    *  allocated draw commands.
    *
    *      struct nk_context ctx;
    *      nk_init_xxx(&ctx, ...);
    *      while (1) {
    *          Event evt;
    *          nk_input_begin(&ctx);
    *          while (GetEvent(&evt)) {
    *              if (evt.type == MOUSE_MOVE)
    *                  nk_input_motion(&ctx, evt.motion.x, evt.motion.y);
    *              else if (evt.type == [...]) {
    *                  [...]
    *              }
    *          }
    *          nk_input_end(&ctx);
    *
    *          [...]
    *
    *          const struct nk_command *cmd = 0;
    *          nk_foreach(cmd, &ctx) {
    *          switch (cmd->type) {
    *          case NK_COMMAND_LINE:
    *              your_draw_line_function(...)
    *              break;
    *          case NK_COMMAND_RECT
    *              your_draw_rect_function(...)
    *              break;
    *          case ...:
    *              [...]
    *          }
    *          nk_clear(&ctx);
    *      }
    *      nk_free(&ctx);
    *
    *  You probably noticed that you have to draw all of the UI each frame which is
    *  quite wasteful. While the actual UI updating loop is quite fast rendering
    *  without actually needing it is not. So there are multiple things you could do.
    *
    *  First is only update on input. This of course is only an option if your
    *  application only depends on the UI and does not require any outside calculations.
    *  If you actually only update on input make sure to update the UI two times each
    *  frame and call `nk_clear` directly after the first pass and only draw in
    *  the second pass.
    *
    *      struct nk_context ctx;
    *      nk_init_xxx(&ctx, ...);
    *      while (1) {
    *          [...wait for input ]
    *
    *          [...do two UI passes ...]
    *          do_ui(...)
    *          nk_clear(&ctx);
    *          do_ui(...)
    *
    *          const struct nk_command *cmd = 0;
    *          nk_foreach(cmd, &ctx) {
    *          switch (cmd->type) {
    *          case NK_COMMAND_LINE:
    *              your_draw_line_function(...)
    *              break;
    *          case NK_COMMAND_RECT
    *              your_draw_rect_function(...)
    *              break;
    *          case ...:
    *              [...]
    *          }
    *          nk_clear(&ctx);
    *      }
    *      nk_free(&ctx);
    *
    *  The second probably more applicable trick is to only draw if anything changed.
    *  It is not really useful for applications with continous draw loop but
    *  quite useful for desktop applications. To actually get nuklear to only
    *  draw on changes you first have to define `NK_ZERO_COMMAND_MEMORY` and
    *  allocate a memory buffer that will store each unique drawing output.
    *  After each frame you compare the draw command memory inside the library
    *  with your allocated buffer by memcmp. If memcmp detects differences
    *  you have to copy the nuklears command buffer into the allocated buffer
    *  and then draw like usual (this example uses fixed memory but you could
    *  use dynamically allocated memory).
    *
    *      [... other defines ...]
    *      #define NK_ZERO_COMMAND_MEMORY
    *      #include "nuklear.h"
    *
    *      struct nk_context ctx;
    *      void *last = calloc(1,64*1024);
    *      void *buf = calloc(1,64*1024);
    *      nk_init_fixed(&ctx, buf, 64*1024);
    *      while (1) {
    *          [...input...]
    *          [...ui...]
    *
    *          void *cmds = nk_buffer_memory(&ctx.memory);
    *          if (memcmp(cmds, last, ctx.memory.allocated)) {
    *              memcpy(last,cmds,ctx.memory.allocated);
    *              const struct nk_command *cmd = 0;
    *              nk_foreach(cmd, &ctx) {
    *                  switch (cmd->type) {
    *                  case NK_COMMAND_LINE:
    *                      your_draw_line_function(...)
    *                      break;
    *                  case NK_COMMAND_RECT
    *                      your_draw_rect_function(...)
    *                      break;
    *                  case ...:
    *                      [...]
    *                  }
    *              }
    *          }
    *          nk_clear(&ctx);
    *      }
    *      nk_free(&ctx);
    *
    *  Finally while using draw commands makes sense for higher abstracted platforms like
    *  X11 and Win32 or drawing libraries it is often desirable to use graphics
    *  hardware directly. Therefore it is possible to just define
    *  `NK_INCLUDE_VERTEX_BUFFER_OUTPUT` which includes optional vertex output.
    *  To access the vertex output you first have to convert all draw commands into
    *  vertexes by calling `nk_convert` which takes in your prefered vertex format.
    *  After successfully converting all draw commands just iterate over and execute all
    *  vertex draw commands:
    *
    *      struct nk_convert_config cfg = {};
    *      static const struct nk_draw_vertex_layout_element vertex_layout[] = {
    *          {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct your_vertex, pos)},
    *          {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct your_vertex, uv)},
    *          {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct your_vertex, col)},
    *          {NK_VERTEX_LAYOUT_END}
    *      };
    *      cfg.shape_AA = NK_ANTI_ALIASING_ON;
    *      cfg.line_AA = NK_ANTI_ALIASING_ON;
    *      cfg.vertex_layout = vertex_layout;
    *      cfg.vertex_size = sizeof(struct your_vertex);
    *      cfg.vertex_alignment = NK_ALIGNOF(struct your_vertex);
    *      cfg.circle_segment_count = 22;
    *      cfg.curve_segment_count = 22;
    *      cfg.arc_segment_count = 22;
    *      cfg.global_alpha = 1.0f;
    *      cfg.null = dev->null;
    *
    *      struct nk_buffer cmds, verts, idx;
    *      nk_buffer_init_default(&cmds);
    *      nk_buffer_init_default(&verts);
    *      nk_buffer_init_default(&idx);
    *      nk_convert(&ctx, &cmds, &verts, &idx, &cfg);
    *      nk_draw_foreach(cmd, &ctx, &cmds) {
    *          if (!cmd->elem_count) continue;
    *          [...]
    *      }
    *      nk_buffer_free(&cms);
    *      nk_buffer_free(&verts);
    *      nk_buffer_free(&idx);
    *
    *  Reference
    *  -------------------
    *  nk__begin           - Returns the first draw command in the context draw command list to be drawn
    *  nk__next            - Increments the draw command iterator to the next command inside the context draw command list
    *  nk_foreach          - Iteratates over each draw command inside the context draw command list
    *
    *  nk_convert          - Converts from the abstract draw commands list into a hardware accessable vertex format
    *  nk__draw_begin      - Returns the first vertex command in the context vertex draw list to be executed
    *  nk__draw_next       - Increments the vertex command iterator to the next command inside the context vertex command list
    *  nk__draw_end        - Returns the end of the vertex draw list
    *  nk_draw_foreach     - Iterates over each vertex draw command inside the vertex draw list
    */
    enum nk_anti_aliasing { NK_ANTI_ALIASING_OFF, NK_ANTI_ALIASING_ON };
    enum nk_convert_result {
        NK_CONVERT_SUCCESS = 0,
        NK_CONVERT_INVALID_PARAM = 1,
        NK_CONVERT_COMMAND_BUFFER_FULL = NK_FLAG(1),
        NK_CONVERT_VERTEX_BUFFER_FULL = NK_FLAG(2),
        NK_CONVERT_ELEMENT_BUFFER_FULL = NK_FLAG(3)
    };
    struct nk_draw_null_texture {
        nk_handle texture; /* texture handle to a texture with a white pixel */
        struct nk_vec2 uv; /* coordinates to a white pixel in the texture  */
    };
    struct nk_convert_config {
        float global_alpha; /* global alpha value */
        enum nk_anti_aliasing line_AA; /* line anti-aliasing flag can be turned off if you are tight on memory */
        enum nk_anti_aliasing shape_AA; /* shape anti-aliasing flag can be turned off if you are tight on memory */
        unsigned circle_segment_count; /* number of segments used for circles: default to 22 */
        unsigned arc_segment_count; /* number of segments used for arcs: default to 22 */
        unsigned curve_segment_count; /* number of segments used for curves: default to 22 */
        struct nk_draw_null_texture null; /* handle to texture with a white pixel for shape drawing */
        const struct nk_draw_vertex_layout_element *vertex_layout; /* describes the vertex output format and packing */
        nk_size vertex_size; /* sizeof one vertex for vertex packing */
        nk_size vertex_alignment; /* vertex alignment: Can be optained by NK_ALIGNOF */
    };
    /*  nk__begin - Returns a draw command list iterator to iterate all draw
    *  commands accumulated over one frame.
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct at the end of a frame
    *  Return values:
    *      draw command pointer pointing to the first command inside the draw command list  */
    NK_API const struct nk_command* nk__begin(struct nk_context*);
    /*  nk__next - Returns a draw command list iterator to iterate all draw
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct at the end of a frame
    *      @cmd must point to an previously a draw command either returned by `nk__begin` or `nk__next`
    *  Return values:
    *      draw command pointer pointing to the next command inside the draw command list  */
    NK_API const struct nk_command* nk__next(struct nk_context*, const struct nk_command*);
    /*  nk_foreach - Iterates over each draw command inside the context draw command list
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct at the end of a frame
    *      @cmd pointer initialized to NULL */
#define nk_foreach(c, ctx) for((c) = nk__begin(ctx); (c) != 0; (c) = nk__next(ctx,c))
#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
    /*  nk_convert - converts all internal draw command into vertex draw commands and fills
    *  three buffers with vertexes, vertex draw commands and vertex indicies. The vertex format
    *  as well as some other configuration values have to be configurated by filling out a
    *  `nk_convert_config` struct.
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct at the end of a frame
    *      @cmds must point to a previously initialized buffer to hold converted vertex draw commands
    *      @vertices must point to a previously initialized buffer to hold all produced verticies
    *      @elements must point to a previously initialized buffer to hold all procudes vertex indicies
    *      @config must point to a filled out `nk_config` struct to configure the conversion process
    *  Returns:
    *      returns NK_CONVERT_SUCCESS on success and a enum nk_convert_result error values if not */
    NK_API nk_flags nk_convert(struct nk_context*, struct nk_buffer *cmds, struct nk_buffer *vertices, struct nk_buffer *elements, const struct nk_convert_config*);
    /*  nk__draw_begin - Returns a draw vertex command buffer iterator to iterate each the vertex draw command buffer
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct at the end of a frame
    *      @buf must point to an previously by `nk_convert` filled out vertex draw command buffer
    *  Return values:
    *      vertex draw command pointer pointing to the first command inside the vertex draw command buffer  */
    NK_API const struct nk_draw_command* nk__draw_begin(const struct nk_context*, const struct nk_buffer*);
    /*  nk__draw_end - Returns the vertex draw command  at the end of the vertex draw command buffer
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct at the end of a frame
    *      @buf must point to an previously by `nk_convert` filled out vertex draw command buffer
    *  Return values:
    *      vertex draw command pointer pointing to the end of the last vertex draw command inside the vertex draw command buffer  */
    NK_API const struct nk_draw_command* nk__draw_end(const struct nk_context*, const struct nk_buffer*);
    /*  nk__draw_next - Increments the the vertex draw command buffer iterator
    *  Parameters:
    *      @cmd must point to an previously either by `nk__draw_begin` or `nk__draw_next` returned vertex draw command
    *      @buf must point to an previously by `nk_convert` filled out vertex draw command buffer
    *      @ctx must point to an previously initialized `nk_context` struct at the end of a frame
    *  Return values:
    *      vertex draw command pointer pointing to the end of the last vertex draw command inside the vertex draw command buffer  */
    NK_API const struct nk_draw_command* nk__draw_next(const struct nk_draw_command*, const struct nk_buffer*, const struct nk_context*);
    /*  nk_draw_foreach - Iterates over each vertex draw command inside a vertex draw command buffer
    *  Parameters:
    *      @cmd nk_draw_command pointer set to NULL
    *      @buf must point to an previously by `nk_convert` filled out vertex draw command buffer
    *      @ctx must point to an previously initialized `nk_context` struct at the end of a frame */
#define nk_draw_foreach(cmd,ctx, b) for((cmd)=nk__draw_begin(ctx, b); (cmd)!=0; (cmd)=nk__draw_next(cmd, b, ctx))
#endif
    /* =============================================================================
    *
    *                                  WINDOW
    *
    * =============================================================================
    * Windows are the main persistent state used inside nuklear and are life time
    * controlled by simply "retouching" (i.e. calling) each window each frame.
    * All widgets inside nuklear can only be added inside function pair `nk_begin_xxx`
    * and `nk_end`. Calling any widgets outside these two functions will result in an
    * assert in debug or no state change in release mode.
    *
    * Each window holds frame persistent state like position, size, flags, state tables,
    * and some garbage collected internal persistent widget state. Each window
    * is linked into a window stack list which determines the drawing and overlapping
    * order. The topmost window thereby is the currently active window.
    *
    * To change window position inside the stack occurs either automatically by
    * user input by being clicked on or programatically by calling `nk_window_focus`.
    * Windows by default are visible unless explicitly being defined with flag
    * `NK_WINDOW_HIDDEN`, the user clicked the close button on windows with flag
    * `NK_WINDOW_CLOSABLE` or if a window was explicitly hidden by calling
    * `nk_window_show`. To explicitly close and destroy a window call `nk_window_close`.
    *
    * Usage
    * -------------------
    * To create and keep a window you have to call one of the two `nk_begin_xxx`
    * functions to start window declarations and `nk_end` at the end. Furthermore it
    * is recommended to check the return value of `nk_begin_xxx` and only process
    * widgets inside the window if the value is not 0. Either way you have to call
    * `nk_end` at the end of window declarations. Furthmore do not attempt to
    * nest `nk_begin_xxx` calls which will hopefully result in an assert or if not
    * in a segmation fault.
    *
    *      if (nk_begin_xxx(...) {
    *          [... widgets ...]
    *      }
    *      nk_end(ctx);
    *
    * In the grand concept window and widget declarations need to occur after input
    * handling and before drawing to screen. Not doing so can result in higher
    * latency or at worst invalid behavior. Furthermore make sure that `nk_clear`
    * is called at the end of the frame. While nuklears default platform backends
    * already call `nk_clear` for you if you write your own backend not calling
    * `nk_clear` can cause asserts or even worse undefined behavior.
    *
    *      struct nk_context ctx;
    *      nk_init_xxx(&ctx, ...);
    *      while (1) {
    *          Event evt;
    *          nk_input_begin(&ctx);
    *          while (GetEvent(&evt)) {
    *              if (evt.type == MOUSE_MOVE)
    *                  nk_input_motion(&ctx, evt.motion.x, evt.motion.y);
    *              else if (evt.type == [...]) {
    *                  nk_input_xxx(...);
    *              }
    *          }
    *          nk_input_end(&ctx);
    *
    *          if (nk_begin_xxx(...) {
    *              [...]
    *          }
    *          nk_end(ctx);
    *
    *          const struct nk_command *cmd = 0;
    *          nk_foreach(cmd, &ctx) {
    *          case NK_COMMAND_LINE:
    *              your_draw_line_function(...)
    *              break;
    *          case NK_COMMAND_RECT
    *              your_draw_rect_function(...)
    *              break;
    *          case ...:
    *              [...]
    *          }
    *          nk_clear(&ctx);
    *      }
    *      nk_free(&ctx);
    *
    *  Reference
    *  -------------------
    *  nk_begin                            - starts a new window; needs to be called every frame for every window (unless hidden) or otherwise the window gets removed
    *  nk_begin_titled                     - extended window start with seperated title and identifier to allow multiple windows with same name but not title
    *  nk_end                              - needs to be called at the end of the window building process to process scaling, scrollbars and general cleanup
    *
    *  nk_window_find                      - finds and returns the window with give name
    *  nk_window_get_bounds                - returns a rectangle with screen position and size of the currently processed window.
    *  nk_window_get_position              - returns the position of the currently processed window
    *  nk_window_get_size                  - returns the size with width and height of the currently processed window
    *  nk_window_get_width                 - returns the width of the currently processed window
    *  nk_window_get_height                - returns the height of the currently processed window
    *  nk_window_get_panel                 - returns the underlying panel which contains all processing state of the currnet window
    *  nk_window_get_content_region        - returns the position and size of the currently visible and non-clipped space inside the currently processed window
    *  nk_window_get_content_region_min    - returns the upper rectangle position of the currently visible and non-clipped space inside the currently processed window
    *  nk_window_get_content_region_max    - returns the upper rectangle position of the currently visible and non-clipped space inside the currently processed window
    *  nk_window_get_content_region_size   - returns the size of the currently visible and non-clipped space inside the currently processed window
    *  nk_window_get_canvas                - returns the draw command buffer. Can be used to draw custom widgets
    *
    *  nk_window_has_focus                 - returns if the currently processed window is currently active
    *  nk_window_is_collapsed              - returns if the window with given name is currently minimized/collapsed
    *  nk_window_is_closed                 - returns if the currently processed window was closed
    *  nk_window_is_hidden                 - returns if the currently processed window was hidden
    *  nk_window_is_active                 - same as nk_window_has_focus for some reason
    *  nk_window_is_hovered                - returns if the currently processed window is currently being hovered by mouse
    *  nk_window_is_any_hovered            - return if any wndow currently hovered
    *  nk_item_is_any_active               - returns if any window or widgets is currently hovered or active
    *
    *  nk_window_set_bounds                - updates position and size of the currently processed window
    *  nk_window_set_position              - updates position of the currently process window
    *  nk_window_set_size                  - updates the size of the currently processed window
    *  nk_window_set_focus                 - set the currently processed window as active window
    *
    *  nk_window_close                     - closes the window with given window name which deletes the window at the end of the frame
    *  nk_window_collapse                  - collapses the window with given window name
    *  nk_window_collapse_if               - collapses the window with given window name if the given condition was met
    *  nk_window_show                      - hides a visible or reshows a hidden window
    *  nk_window_show_if                   - hides/shows a window depending on condition
    */
    enum nk_panel_flags {
        NK_WINDOW_BORDER = NK_FLAG(0), /* Draws a border around the window to visually separate window from the background */
        NK_WINDOW_MOVABLE = NK_FLAG(1), /* The movable flag indicates that a window can be moved by user input or by dragging the window header */
        NK_WINDOW_SCALABLE = NK_FLAG(2), /* The scalable flag indicates that a window can be scaled by user input by dragging a scaler icon at the button of the window */
        NK_WINDOW_CLOSABLE = NK_FLAG(3), /* adds a closable icon into the header */
        NK_WINDOW_MINIMIZABLE = NK_FLAG(4), /* adds a minimize icon into the header */
        NK_WINDOW_NO_SCROLLBAR = NK_FLAG(5), /* Removes the scrollbar from the window */
        NK_WINDOW_TITLE = NK_FLAG(6), /* Forces a header at the top at the window showing the title */
        NK_WINDOW_SCROLL_AUTO_HIDE = NK_FLAG(7), /* Automatically hides the window scrollbar if no user interaction: also requires delta time in `nk_context` to be set each frame */
        NK_WINDOW_BACKGROUND = NK_FLAG(8), /* Always keep window in the background */
        NK_WINDOW_SCALE_LEFT = NK_FLAG(9), /* Puts window scaler in the left-ottom corner instead right-bottom*/
        NK_WINDOW_NO_INPUT = NK_FLAG(10) /* Prevents window of scaling, moving or getting focus */
    };
    /*  nk_begin - starts a new window; needs to be called every frame for every window (unless hidden) or otherwise the window gets removed
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @title window title and identifier. Needs to be persitent over frames to identify the window
    *      @bounds initial position and window size. However if you do not define `NK_WINDOW_SCALABLE` or `NK_WINDOW_MOVABLE` you can set window position and size every frame
    *      @flags window flags defined in `enum nk_panel_flags` with a number of different window behaviors
    *  Return values:
    *      returns 1 if the window can be filled up with widgets from this point until `nk_end or 0 otherwise for example if minimized `*/
    NK_API int nk_begin(struct nk_context *ctx, const char *title, struct nk_rect bounds, nk_flags flags);
    /*  nk_begin_titled - extended window start with seperated title and identifier to allow multiple windows with same name but not title
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name window identifier. Needs to be persitent over frames to identify the window
    *      @title window title displayed inside header if flag `NK_WINDOW_TITLE` or either `NK_WINDOW_CLOSABLE` or `NK_WINDOW_MINIMIZED` was set
    *      @bounds initial position and window size. However if you do not define `NK_WINDOW_SCALABLE` or `NK_WINDOW_MOVABLE` you can set window position and size every frame
    *      @flags window flags defined in `enum nk_panel_flags` with a number of different window behaviors
    *  Return values:
    *      returns 1 if the window can be filled up with widgets from this point until `nk_end or 0 otherwise `*/
    NK_API int nk_begin_titled(struct nk_context *ctx, const char *name, const char *title, struct nk_rect bounds, nk_flags flags);
    /*  nk_end - needs to be called at the end of the window building process to process scaling, scrollbars and general cleanup.
    *  All widget calls after this functions will result in asserts or no state changes
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct */
    NK_API void nk_end(struct nk_context *ctx);
    /*  nk_window_find - finds and returns the window with give name
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name window identifier
    *  Return values:
    *      returns a `nk_window` struct pointing to the idified window or 0 if no window with given name was found */
    NK_API struct nk_window *nk_window_find(struct nk_context *ctx, const char *name);
    /*  nk_window_get_bounds - returns a rectangle with screen position and size of the currently processed window.
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns a `nk_rect` struct with window upper left position and size */
    NK_API struct nk_rect nk_window_get_bounds(const struct nk_context *ctx);
    /*  nk_window_get_position - returns the position of the currently processed window.
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns a `nk_vec2` struct with window upper left position */
    NK_API struct nk_vec2 nk_window_get_position(const struct nk_context *ctx);
    /*  nk_window_get_size - returns the size with width and height of the currently processed window.
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns a `nk_vec2` struct with window size */
    NK_API struct nk_vec2 nk_window_get_size(const struct nk_context*);
    /*  nk_window_get_width - returns the width of the currently processed window.
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns the window width */
    NK_API float nk_window_get_width(const struct nk_context*);
    /*  nk_window_get_height - returns the height of the currently processed window.
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns the window height */
    NK_API float nk_window_get_height(const struct nk_context*);
    /*  nk_window_get_panel - returns the underlying panel which contains all processing state of the currnet window.
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns a pointer to window internal `nk_panel` state. DO NOT keep this pointer around it is only valid until `nk_end` */
    NK_API struct nk_panel* nk_window_get_panel(struct nk_context*);
    /*  nk_window_get_content_region - returns the position and size of the currently visible and non-clipped space inside the currently processed window.
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns `nk_rect` struct with screen position and size (no scrollbar offset) of the visible space inside the current window */
    NK_API struct nk_rect nk_window_get_content_region(struct nk_context*);
    /*  nk_window_get_content_region_min - returns the upper left position of the currently visible and non-clipped space inside the currently processed window.
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns `nk_vec2` struct with  upper left screen position (no scrollbar offset) of the visible space inside the current window */
    NK_API struct nk_vec2 nk_window_get_content_region_min(struct nk_context*);
    /*  nk_window_get_content_region_max - returns the lower right screen position of the currently visible and non-clipped space inside the currently processed window.
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns `nk_vec2` struct with lower right screen position (no scrollbar offset) of the visible space inside the current window */
    NK_API struct nk_vec2 nk_window_get_content_region_max(struct nk_context*);
    /*  nk_window_get_content_region_size - returns the size of the currently visible and non-clipped space inside the currently processed window
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns `nk_vec2` struct with size the visible space inside the current window */
    NK_API struct nk_vec2 nk_window_get_content_region_size(struct nk_context*);
    /*  nk_window_get_canvas - returns the draw command buffer. Can be used to draw custom widgets
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns a pointer to window internal `nk_command_buffer` struct used as drawing canvas. Can be used to do custom drawing */
    NK_API struct nk_command_buffer* nk_window_get_canvas(struct nk_context*);
    /*  nk_window_has_focus - returns if the currently processed window is currently active
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns 0 if current window is not active or 1 if it is */
    NK_API int nk_window_has_focus(const struct nk_context*);
    /*  nk_window_is_collapsed - returns if the window with given name is currently minimized/collapsed
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name of window you want to check is collapsed
    *  Return values:
    *      returns 1 if current window is minimized and 0 if window not found or is not minimized */
    NK_API int nk_window_is_collapsed(struct nk_context *ctx, const char *name);
    /*  nk_window_is_closed - returns if the window with given name was closed by calling `nk_close`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name of window you want to check is closed
    *  Return values:
    *      returns 1 if current window was closed or 0 window not found or not closed */
    NK_API int nk_window_is_closed(struct nk_context*, const char*);
    /*  nk_window_is_hidden - returns if the window with given name is hidden
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name of window you want to check is hidden
    *  Return values:
    *      returns 1 if current window is hidden or 0 window not found or visible */
    NK_API int nk_window_is_hidden(struct nk_context*, const char*);
    /*  nk_window_is_active - same as nk_window_has_focus for some reason
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name of window you want to check is hidden
    *  Return values:
    *      returns 1 if current window is active or 0 window not found or not active */
    NK_API int nk_window_is_active(struct nk_context*, const char*);
    /*  nk_window_is_hovered - return if the current window is being hovered
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns 1 if current window is hovered or 0 otherwise */
    NK_API int nk_window_is_hovered(struct nk_context*);
    /*  nk_window_is_any_hovered - returns if the any window is being hovered
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns 1 if any window is hovered or 0 otherwise */
    NK_API int nk_window_is_any_hovered(struct nk_context*);
    /*  nk_item_is_any_active - returns if the any window is being hovered or any widget is currently active.
    *  Can be used to decide if input should be processed by UI or your specific input handling.
    *  Example could be UI and 3D camera to move inside a 3D space.
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *  Return values:
    *      returns 1 if any window is hovered or any item is active or 0 otherwise */
    NK_API int nk_item_is_any_active(struct nk_context*);
    /*  nk_window_set_bounds - updates position and size of the currently processed window
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @bounds points to a `nk_rect` struct with the new position and size of currently active window */
    NK_API void nk_window_set_bounds(struct nk_context*, struct nk_rect bounds);
    /*  nk_window_set_position - updates position of the currently processed window
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @pos points to a `nk_vec2` struct with the new position of currently active window */
    NK_API void nk_window_set_position(struct nk_context*, struct nk_vec2 pos);
    /*  nk_window_set_size - updates size of the currently processed window
    *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @bounds points to a `nk_vec2` struct with the new size of currently active window */
    NK_API void nk_window_set_size(struct nk_context*, struct nk_vec2);
    /*  nk_window_set_focus - sets the window with given name as active
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name of the window to be set active */
    NK_API void nk_window_set_focus(struct nk_context*, const char *name);
    /*  nk_window_close - closed a window and marks it for being freed at the end of the frame
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name of the window to be closed */
    NK_API void nk_window_close(struct nk_context *ctx, const char *name);
    /*  nk_window_collapse - updates collapse state of a window with given name
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name of the window to be either collapse or maximize */
    NK_API void nk_window_collapse(struct nk_context*, const char *name, enum nk_collapse_states state);
    /*  nk_window_collapse - updates collapse state of a window with given name if given condition is met
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name of the window to be either collapse or maximize
    *      @state the window should be put into
    *      @condition that has to be true to actually commit the collsage state change */
    NK_API void nk_window_collapse_if(struct nk_context*, const char *name, enum nk_collapse_states, int cond);
    /*  nk_window_show - updates visibility state of a window with given name
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name of the window to be either collapse or maximize
    *      @state with either visible or hidden to modify the window with */
    NK_API void nk_window_show(struct nk_context*, const char *name, enum nk_show_states);
    /*  nk_window_show_if - updates visibility state of a window with given name if a given condition is met
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct
    *      @name of the window to be either collapse or maximize
    *      @state with either visible or hidden to modify the window with
    *      @condition that has to be true to actually commit the visible state change */
    NK_API void nk_window_show_if(struct nk_context*, const char *name, enum nk_show_states, int cond);
    /* =============================================================================
    *
    *                                  LAYOUT
    *
    * ============================================================================= */
    /*  Layouting in general describes placing widget inside a window with position and size.
    *  While in this particular implementation there are five different APIs for layouting
    *  each with different trade offs between control and ease of use.
    *
    *  All layouting methodes in this library are based around the concept of a row.
    *  A row has a height the window content grows by and a number of columns and each
    *  layouting method specifies how each widget is placed inside the row.
    *  After a row has been allocated by calling a layouting functions and then
    *  filled with widgets will advance an internal pointer over the allocated row.
    *
    *  To acually define a layout you just call the appropriate layouting function
    *  and each subsequnetial widget call will place the widget as specified. Important
    *  here is that if you define more widgets then columns defined inside the layout
    *  functions it will allocate the next row without you having to make another layouting
    *  call.
    *
    *  Biggest limitation with using all these APIs outside the `nk_layout_space_xxx` API
    *  is that you have to define the row height for each. However the row height
    *  often depends on the height of the font. So I would recommend writing
    *  a higher level layouting functions that just calls each function with default font height
    *  plus some spacing between rows. The reason why this library does't support this
    *  behavior by default is to grant more control.
    *
    *  For actually more advanced UI I would even recommend using the `nk_layout_space_xxx`
    *  layouting method in combination with a cassowary constraint solver (there are
    *  some versions on github with permissive license model) to take over all control over widget
    *  layouting yourself. However for quick and dirty layouting using all the other layouting
    *  functions, especially if you don't change the font height, should be fine.
    *
    *  Usage
    *  -------------------
    *  1.) nk_layout_row_dynamic
    *  The easiest layouting function is `nk_layout_row_dynamic`. It provides each
    *  widgets with same horizontal space inside the row and dynamically grows
    *  if the owning window grows in width. So the number of columns dictates
    *  the size of each widget dynamically by formula:
    *
    *      widget_width = (window_width - padding - spacing) * (1/colum_count)
    *
    *  Just like all other layouting APIs if you define more widget than columns this
    *  library will allocate a new row and keep all layouting parameters previously
    *  defined.
    *
    *      if (nk_begin_xxx(...) {
    *          // first row with height: 30 composed of two widgets
    *          nk_layout_row_dynamic(&ctx, 30, 2);
    *          nk_widget(...);
    *          nk_widget(...);
    *
    *          // second row with same parameter as defined above
    *          nk_widget(...);
    *          nk_widget(...);
    *      }
    *      nk_end(...);
    *
    *  2.) nk_layout_row_static
    *  Another easy layouting function is `nk_layout_row_static`. It provides each
    *  widget with same horizontal pixel width inside the row and does not grow
    *  if the owning window scales smaller or bigger.
    *
    *      if (nk_begin_xxx(...) {
    *          // first row with height: 30 composed of two widgets with width: 80
    *          nk_layout_row_static(&ctx, 30, 80, 2);
    *          nk_widget(...);
    *          nk_widget(...);
    *
    *          // second row with same parameter as defined above
    *          nk_widget(...);
    *          nk_widget(...);
    *      }
    *      nk_end(...);
    *
    *  3.) nk_layout_row_xxx
    *  A little bit more advanced layouting API are functions `nk_layout_row_begin`,
    *  `nk_layout_row_push` and `nk_layout_row_end`. They allow to directly
    *  specify each column pixel or window ratio in a row. It support either
    *  directly setting per column pixel width or widget window ratio but not
    *  both. Furthermore it is a immediate mode API so each value is directly
    *  pushed before calling a widget. Therefore the layout is not automatically
    *  repeating like the last two layouting functions.
    *
    *      if (nk_begin_xxx(...) {
    *          // first row with height: 25 composed of two widgets with width 60 and 40
    *          nk_layout_row_begin(ctx, NK_STATIC, 25, 2);
    *          nk_layout_row_push(ctx, 60);
    *          nk_widget(...);
    *          nk_layout_row_push(ctx, 40);
    *          nk_widget(...);
    *          nk_layout_row_end(ctx);
    *
    *          // second row with height: 25 composed of two widgets with window ratio 0.25 and 0.75
    *          nk_layout_row_begin(ctx, NK_DYNAMIC, 25, 2);
    *          nk_layout_row_push(ctx, 0.25f);
    *          nk_widget(...);
    *          nk_layout_row_push(ctx, 0.75f);
    *          nk_widget(...);
    *          nk_layout_row_end(ctx);
    *      }
    *      nk_end(...);
    *
    *  4.) nk_layout_row
    *  The retain mode counterpart to API nk_layout_row_xxx is the single nk_layout_row
    *  functions. Instead of pushing either pixel or window ratio for every widget
    *  it allows to define it by array. The trade of for less control is that
    *  `nk_layout_row` is automatically repeating. Otherwise the behavior is the
    *  same.
    *
    *      if (nk_begin_xxx(...) {
    *          // two rows with height: 30 composed of two widgets with width 60 and 40
    *          const float size[] = {60,40};
    *          nk_layout_row(ctx, NK_STATIC, 30, 2, ratio);
    *          nk_widget(...);
    *          nk_widget(...);
    *          nk_widget(...);
    *          nk_widget(...);
    *
    *          // two rows with height: 30 composed of two widgets with window ratio 0.25 and 0.75
    *          const float ratio[] = {0.25, 0.75};
    *          nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratio);
    *          nk_widget(...);
    *          nk_widget(...);
    *          nk_widget(...);
    *          nk_widget(...);
    *      }
    *      nk_end(...);
    *
    *  5.) nk_layout_row_template_xxx
    *  The most complex and second most flexible API is a simplified flexbox version without
    *  line wrapping and weights for dynamic widgets. It is an immediate mode API but
    *  unlike `nk_layout_row_xxx` it has auto repeat behavior and needs to be called
    *  before calling the templated widgets.
    *  The row template layout has three different per widget size specifier. The first
    *  one is the static widget size specifier with fixed widget pixel width. They do
    *  not grow if the row grows and will always stay the same. The second size
    *  specifier is nk_layout_row_template_push_variable which defines a
    *  minumum widget size but it also can grow if more space is available not taken
    *  by other widgets. Finally there are dynamic widgets which are completly flexible
    *  and unlike variable widgets can even shrink to zero if not enough space
    *  is provided.
    *
    *      if (nk_begin_xxx(...) {
    *          // two rows with height: 30 composed of three widgets
    *          nk_layout_row_template_begin(ctx, 30);
    *          nk_layout_row_template_push_dynamic(ctx);
    *          nk_layout_row_template_push_variable(ctx, 80);
    *          nk_layout_row_template_push_static(ctx, 80);
    *          nk_layout_row_template_end(ctx);
    *
    *          nk_widget(...); // dynamic widget completly can go to zero
    *          nk_widget(...); // variable widget with min 80 pixel but can grow bigger if enough space
    *          nk_widget(...); // static widget with fixed 80 pixel width
    *      }
    *      nk_end(...);
    *
    *  6.) nk_layout_space_xxx
    *  Finally the most flexible API directly allows you to place widgets inside the
    *  window. The space layout API is an immediate mode API which does not support
    *  row auto repeat and directly sets position and size of a widget. Position
    *  and size hereby can be either specified as ratio of alloated space or
    *  allocated space local position and pixel size. Since this API is quite
    *  powerfull there are a number of utility functions to get the available space
    *  and convert between local allocated space and screen space.
    *
    *      if (nk_begin_xxx(...) {
    *          // static row with height: 500 (you can set column count to INT_MAX if you don't want to be bothered)
    *          nk_layout_space_begin(ctx, NK_STATIC, 500, INT_MAX);
    *          nk_layout_space_push(ctx, nk_rect(0,0,150,200));
    *          nk_widget(...);
    *          nk_layout_space_push(ctx, nk_rect(200,200,100,200));
    *          nk_widget(...);
    *          nk_layout_space_end(ctx);
    *
    *          // dynamic row with height: 500 (you can set column count to INT_MAX if you don't want to be bothered)
    *          nk_layout_space_begin(ctx, NK_DYNAMIC, 500, INT_MAX);
    *          nk_layout_space_push(ctx, nk_rect(0.5,0.5,0.1,0.1));
    *          nk_widget(...);
    *          nk_layout_space_push(ctx, nk_rect(0.7,0.6,0.1,0.1));
    *          nk_widget(...);
    *      }
    *      nk_end(...);
    *
    *  Reference
    *  -------------------
    *  nk_layout_row_dynamic           - current layout is divided into n same sized gowing columns
    *  nk_layout_row_static            - current layout is divided into n same fixed sized columns
    *  nk_layout_row_begin             - starts a new row with given height and number of columns
    *  nk_layout_row_push              - pushes another column with given size or window ratio
    *  nk_layout_row_end               - finished previously started row
    *  nk_layout_row                   - specifies row columns in array as either window ratio or size
    *
    *  nk_layout_row_template_begin
    *  nk_layout_row_template_push_dynamic
    *  nk_layout_row_template_push_variable
    *  nk_layout_row_template_push_static
    *  nk_layout_row_template_end
    *
    *  nk_layout_space_begin
    *  nk_layout_space_push
    *  nk_layout_space_end
    *
    *  nk_layout_space_bounds
    *  nk_layout_space_to_screen
    *  nk_layout_space_to_local
    *  nk_layout_space_rect_to_screen
    *  nk_layout_space_rect_to_local
    *  nk_layout_ratio_from_pixel
    */
    /*  nk_layout_row_dynamic - Sets current row layout to share horizontal space
    *  between @cols number of widgets evenly. Once called all subsequent widget
    *  calls greater than @cols will allocate a new row with same layout.
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct after call `nk_begin_xxx`
    *      @height holds row height to allocate from panel for widget height
    *      @cols number of widget inside row */
    NK_API void nk_layout_row_dynamic(struct nk_context *ctx, float height, int cols);
    /*  nk_layout_row_static - Sets current row layout to fill @cols number of widgets
    *  in row with same @item_width horizontal size. Once called all subsequent widget
    *  calls greater than @cols will allocate a new row with same layout.
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct after call `nk_begin_xxx`
    *      @height holds row height to allocate from panel for widget height
    *      @item_width holds width of each widget in row
    *      @cols number of widget inside row */
    NK_API void nk_layout_row_static(struct nk_context *ctx, float height, int item_width, int cols);
    /*  nk_layout_row_begin - Starts a new dynamic or fixed row with given height and columns.
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct after call `nk_begin_xxx`
    *      @fmt either `NK_DYNAMIC` for window ratio or `NK_STATIC` for fixed size columns
    *      @row_height holds width of each widget in row
    *      @cols number of widget inside row */
    NK_API void nk_layout_row_begin(struct nk_context *ctx, enum nk_layout_format fmt, float row_height, int cols);
    /*  nk_layout_row_push - Specifies either window ratio or width of a single column
    *  Parameters:
    *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_row_begin`
    *      @value either a window ratio or fixed width depending on @fmt in previous `nk_layout_row_begin` call */
    NK_API void nk_layout_row_push(struct nk_context*, float value);
    NK_API void nk_layout_row_end(struct nk_context*);
    NK_API void nk_layout_row(struct nk_context*, enum nk_layout_format, float height, int cols, const float *ratio);

    NK_API void nk_layout_row_template_begin(struct nk_context*, float height);
    NK_API void nk_layout_row_template_push_dynamic(struct nk_context*);
    NK_API void nk_layout_row_template_push_variable(struct nk_context*, float min_width);
    NK_API void nk_layout_row_template_push_static(struct nk_context*, float width);
    // JSandusky: pushes a static part into a row template with a forced vertical alignment
    NK_API void nk_layout_row_template_push_valigned_static(struct nk_context*, float width, float height, nk_layout_valign vAlign);
    NK_API void nk_layout_row_template_end(struct nk_context*);

    NK_API void nk_layout_space_begin(struct nk_context*, enum nk_layout_format, float height, int widget_count);
    NK_API void nk_layout_space_push(struct nk_context*, struct nk_rect);
    NK_API void nk_layout_space_end(struct nk_context*);

    NK_API struct nk_rect nk_layout_space_bounds(struct nk_context*);
    NK_API struct nk_vec2 nk_layout_space_to_screen(struct nk_context*, struct nk_vec2);
    NK_API struct nk_vec2 nk_layout_space_to_local(struct nk_context*, struct nk_vec2);
    NK_API struct nk_rect nk_layout_space_rect_to_screen(struct nk_context*, struct nk_rect);
    NK_API struct nk_rect nk_layout_space_rect_to_local(struct nk_context*, struct nk_rect);
    NK_API float nk_layout_ratio_from_pixel(struct nk_context*, float pixel_width);
    /* =============================================================================
    *
    *                                  GROUP
    *
    * ============================================================================= */
    NK_API int nk_group_begin(struct nk_context*, const char *title, nk_flags);
    NK_API int nk_group_scrolled_offset_begin(struct nk_context*, nk_uint *x_offset, nk_uint *y_offset, const char*, nk_flags);
    NK_API int nk_group_scrolled_begin(struct nk_context*, struct nk_scroll*, const char *title, nk_flags);
    NK_API void nk_group_scrolled_end(struct nk_context*);
    NK_API void nk_group_end(struct nk_context*);
    /* =============================================================================
    *
    *                                  LIST VIEW
    *
    * ============================================================================= */
    struct nk_list_view {
        /* public: */
        int begin, end, count;
        /* private: */
        int total_height;
        struct nk_context *ctx;
        nk_uint *scroll_pointer;
        nk_uint scroll_value;
    };
    NK_API int nk_list_view_begin(struct nk_context*, struct nk_list_view *out, const char *id, nk_flags, int row_height, int row_count);
    NK_API void nk_list_view_end(struct nk_list_view*);
    /* =============================================================================
    *
    *                                  TREE
    *
    * ============================================================================= */
#define nk_tree_push(ctx, type, title, state) nk_tree_push_hashed(ctx, type, title, state, NK_FILE_LINE,nk_strlen(NK_FILE_LINE),__LINE__)
#define nk_tree_push_id(ctx, type, title, state, id) nk_tree_push_hashed(ctx, type, title, state, NK_FILE_LINE,nk_strlen(NK_FILE_LINE),id)
    NK_API int nk_tree_push_hashed(struct nk_context*, enum nk_tree_type, const char *title, enum nk_collapse_states initial_state, const char *hash, int len, int seed);
#define nk_tree_image_push(ctx, type, img, title, state) nk_tree_image_push_hashed(ctx, type, img, title, state, NK_FILE_LINE,nk_strlen(NK_FILE_LINE),__LINE__)
#define nk_tree_image_push_id(ctx, type, img, title, state, id) nk_tree_image_push_hashed(ctx, type, img, title, state, NK_FILE_LINE,nk_strlen(NK_FILE_LINE),id)
    NK_API int nk_tree_image_push_hashed(struct nk_context*, enum nk_tree_type, struct nk_image, const char *title, enum nk_collapse_states initial_state, const char *hash, int len, int seed);
    NK_API void nk_tree_pop(struct nk_context*);
    NK_API int nk_tree_state_push(struct nk_context*, enum nk_tree_type, const char *title, enum nk_collapse_states *state);
    NK_API int nk_tree_state_image_push(struct nk_context*, enum nk_tree_type, struct nk_image, const char *title, enum nk_collapse_states *state);
    NK_API void nk_tree_state_pop(struct nk_context*);
    /* =============================================================================
    *
    *                                  WIDGET
    *
    * ============================================================================= */
    enum nk_widget_layout_states {
        NK_WIDGET_INVALID, /* The widget cannot be seen and is completely out of view */
        NK_WIDGET_VALID, /* The widget is completely inside the window and can be updated and drawn */
        NK_WIDGET_ROM /* The widget is partially visible and cannot be updated */
    };
    enum nk_widget_states {
        NK_WIDGET_STATE_MODIFIED = NK_FLAG(1),
        NK_WIDGET_STATE_INACTIVE = NK_FLAG(2), /* widget is neither active nor hovered */
        NK_WIDGET_STATE_ENTERED = NK_FLAG(3), /* widget has been hovered on the current frame */
        NK_WIDGET_STATE_HOVER = NK_FLAG(4), /* widget is being hovered */
        NK_WIDGET_STATE_ACTIVED = NK_FLAG(5),/* widget is currently activated */
        NK_WIDGET_STATE_LEFT = NK_FLAG(6), /* widget is from this frame on not hovered anymore */
        NK_WIDGET_STATE_HOVERED = NK_WIDGET_STATE_HOVER | NK_WIDGET_STATE_MODIFIED, /* widget is being hovered */
        NK_WIDGET_STATE_ACTIVE = NK_WIDGET_STATE_ACTIVED | NK_WIDGET_STATE_MODIFIED, /* widget is currently activated */
        NK_WIDGET_STATE_KEYBOARD_FOCUS = NK_WIDGET_STATE_HOVER | NK_WIDGET_STATE_ACTIVE | NK_FLAG(7),
    };
    NK_API enum nk_widget_layout_states nk_widget(struct nk_rect*, const struct nk_context*);
    NK_API enum nk_widget_layout_states nk_widget_fitting(struct nk_rect*, struct nk_context*, struct nk_vec2);
    NK_API struct nk_rect nk_widget_bounds(struct nk_context*);
    NK_API struct nk_vec2 nk_widget_position(struct nk_context*);
    NK_API struct nk_vec2 nk_widget_size(struct nk_context*);
    NK_API float nk_widget_width(struct nk_context*);
    NK_API float nk_widget_height(struct nk_context*);
    NK_API int nk_widget_is_hovered(struct nk_context*);
    NK_API int nk_widget_is_mouse_clicked(struct nk_context*, enum nk_buttons);
    NK_API int nk_widget_has_mouse_click_down(struct nk_context*, enum nk_buttons, int down);
    NK_API void nk_spacing(struct nk_context*, int cols);
    /* =============================================================================
    *
    *                                  TEXT
    *
    * ============================================================================= */
    enum nk_text_align {
        NK_TEXT_ALIGN_LEFT = 0x01,
        NK_TEXT_ALIGN_CENTERED = 0x02,
        NK_TEXT_ALIGN_RIGHT = 0x04,
        NK_TEXT_ALIGN_TOP = 0x08,
        NK_TEXT_ALIGN_MIDDLE = 0x10,
        NK_TEXT_ALIGN_BOTTOM = 0x20
    };
    enum nk_text_alignment {
        NK_TEXT_LEFT = NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_LEFT,
        NK_TEXT_CENTERED = NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_CENTERED,
        NK_TEXT_RIGHT = NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_RIGHT
    };
    NK_API void nk_text(struct nk_context*, const char*, int, nk_flags);
    NK_API void nk_text_colored(struct nk_context*, const char*, int, nk_flags, struct nk_color);
    NK_API void nk_text_wrap(struct nk_context*, const char*, int);
    NK_API void nk_text_wrap_colored(struct nk_context*, const char*, int, struct nk_color);
    NK_API void nk_label(struct nk_context*, const char*, nk_flags align);
    NK_API void nk_label_colored(struct nk_context*, const char*, nk_flags align, struct nk_color);
    NK_API void nk_label_wrap(struct nk_context*, const char*);
    NK_API void nk_label_colored_wrap(struct nk_context*, const char*, struct nk_color);
    NK_API void nk_image(struct nk_context*, struct nk_image);
#ifdef NK_INCLUDE_STANDARD_VARARGS
    NK_API void nk_labelf(struct nk_context*, nk_flags, const char*, ...);
    NK_API void nk_labelf_colored(struct nk_context*, nk_flags align, struct nk_color, const char*, ...);
    NK_API void nk_labelf_wrap(struct nk_context*, const char*, ...);
    NK_API void nk_labelf_colored_wrap(struct nk_context*, struct nk_color, const char*, ...);
    NK_API void nk_value_bool(struct nk_context*, const char *prefix, int);
    NK_API void nk_value_int(struct nk_context*, const char *prefix, int);
    NK_API void nk_value_uint(struct nk_context*, const char *prefix, unsigned int);
    NK_API void nk_value_float(struct nk_context*, const char *prefix, float);
    NK_API void nk_value_color_byte(struct nk_context*, const char *prefix, struct nk_color);
    NK_API void nk_value_color_float(struct nk_context*, const char *prefix, struct nk_color);
    NK_API void nk_value_color_hex(struct nk_context*, const char *prefix, struct nk_color);
#endif
    /* =============================================================================
    *
    *                                  BUTTON
    *
    * ============================================================================= */
    NK_API int nk_button_text(struct nk_context*, const char *title, int len);
    NK_API int nk_button_label(struct nk_context*, const char *title);
    NK_API int nk_button_color(struct nk_context*, struct nk_color);
    NK_API int nk_button_symbol(struct nk_context*, enum nk_symbol_type);
    NK_API int nk_button_image(struct nk_context*, struct nk_image img);
    NK_API int nk_button_symbol_label(struct nk_context*, enum nk_symbol_type, const char*, nk_flags text_alignment);
    NK_API int nk_button_symbol_text(struct nk_context*, enum nk_symbol_type, const char*, int, nk_flags alignment);
    NK_API int nk_button_image_label(struct nk_context*, struct nk_image img, const char*, nk_flags text_alignment);
    NK_API int nk_button_image_text(struct nk_context*, struct nk_image img, const char*, int, nk_flags alignment);
    NK_API int nk_button_text_styled(struct nk_context*, const struct nk_style_button*, const char *title, int len);
    NK_API int nk_button_label_styled(struct nk_context*, const struct nk_style_button*, const char *title);
    NK_API int nk_button_symbol_styled(struct nk_context*, const struct nk_style_button*, enum nk_symbol_type);
    NK_API int nk_button_image_styled(struct nk_context*, const struct nk_style_button*, struct nk_image img);
    NK_API int k_button_symbol_label_styled(struct nk_context*, const struct nk_style_button*, enum nk_symbol_type, const char*, nk_flags text_alignment);
    NK_API int nk_button_symbol_text_styled(struct nk_context*, const struct nk_style_button*, enum nk_symbol_type, const char*, int, nk_flags alignment);
    NK_API int nk_button_symbol_label_styled(struct nk_context *ctx, const struct nk_style_button *style, enum nk_symbol_type symbol, const char *title, nk_flags align);
    NK_API int nk_button_image_label_styled(struct nk_context*, const struct nk_style_button*, struct nk_image img, const char*, nk_flags text_alignment);
    NK_API int nk_button_image_text_styled(struct nk_context*, const struct nk_style_button*, struct nk_image img, const char*, int, nk_flags alignment);
    NK_API void nk_button_set_behavior(struct nk_context*, enum nk_button_behavior);
    NK_API int nk_button_push_behavior(struct nk_context*, enum nk_button_behavior);
    NK_API int nk_button_pop_behavior(struct nk_context*);
    /* =============================================================================
    *
    *                                  CHECKBOX
    *
    * ============================================================================= */
    NK_API int nk_check_label(struct nk_context*, const char*, int active);
    NK_API int nk_check_text(struct nk_context*, const char*, int, int active);
    NK_API unsigned nk_check_flags_label(struct nk_context*, const char*, unsigned int flags, unsigned int value);
    NK_API unsigned nk_check_flags_text(struct nk_context*, const char*, int, unsigned int flags, unsigned int value);
    NK_API int nk_checkbox_label(struct nk_context*, const char*, int *active);
    NK_API int nk_checkbox_text(struct nk_context*, const char*, int, int *active);
    NK_API int nk_checkbox_flags_label(struct nk_context*, const char*, unsigned int *flags, unsigned int value);
    NK_API int nk_checkbox_flags_text(struct nk_context*, const char*, int, unsigned int *flags, unsigned int value);
    /* =============================================================================
    *
    *                                  RADIO BUTTON
    *
    * ============================================================================= */
    NK_API int nk_radio_label(struct nk_context*, const char*, int *active);
    NK_API int nk_radio_text(struct nk_context*, const char*, int, int *active);
    NK_API int nk_option_label(struct nk_context*, const char*, int active);
    NK_API int nk_option_text(struct nk_context*, const char*, int, int active);
    /* =============================================================================
    *
    *                                  SELECTABLE
    *
    * ============================================================================= */
    NK_API int nk_selectable_label(struct nk_context*, const char*, nk_flags align, int *value);
    NK_API int nk_selectable_text(struct nk_context*, const char*, int, nk_flags align, int *value);
    NK_API int nk_selectable_image_label(struct nk_context*, struct nk_image, const char*, nk_flags align, int *value);
    NK_API int nk_selectable_image_text(struct nk_context*, struct nk_image, const char*, int, nk_flags align, int *value);
    NK_API int nk_select_label(struct nk_context*, const char*, nk_flags align, int value);
    NK_API int nk_select_text(struct nk_context*, const char*, int, nk_flags align, int value);
    NK_API int nk_select_image_label(struct nk_context*, struct nk_image, const char*, nk_flags align, int value);
    NK_API int nk_select_image_text(struct nk_context*, struct nk_image, const char*, int, nk_flags align, int value);
    /* =============================================================================
    *
    *                                  SLIDER
    *
    * ============================================================================= */
    NK_API float nk_slide_float(struct nk_context*, float min, float val, float max, float step);
    NK_API int nk_slide_int(struct nk_context*, int min, int val, int max, int step);
    NK_API int nk_slider_float(struct nk_context*, float min, float *val, float max, float step);
    NK_API int nk_slider_int(struct nk_context*, int min, int *val, int max, int step);
    /* =============================================================================
    *
    *                                  PROGRESSBAR
    *
    * ============================================================================= */
    NK_API int nk_progress(struct nk_context*, nk_size *cur, nk_size max, int modifyable);
    NK_API nk_size nk_prog(struct nk_context*, nk_size cur, nk_size max, int modifyable);

    /* =============================================================================
    *
    *                                  COLOR PICKER
    *
    * ============================================================================= */
    NK_API struct nk_color nk_color_picker(struct nk_context*, struct nk_color, enum nk_color_format);
    NK_API int nk_color_pick(struct nk_context*, struct nk_color*, enum nk_color_format);
    /* =============================================================================
    *
    *                                  PROPERTIES
    *
    * ============================================================================= */
    enum nk_prop_color_flags {
        NK_PROP_COLOR_OFF = 0,
        NK_PROP_COLOR_PERCENT = 1,
        NK_PROP_COLOR_FULL = 1,
    };
    NK_API int nk_property_int(struct nk_context*, const char *name, int min, int *val, int max, int step, float inc_per_pixel, nk_prop_color_flags flags, nk_color color, int forceOn = 0);
    NK_API int nk_property_float(struct nk_context*, const char *name, float min, float *val, float max, float step, float inc_per_pixel, nk_prop_color_flags flags, nk_color color, int forceOn = 0);
    NK_API int nk_property_double(struct nk_context*, const char *name, double min, double *val, double max, double step, float inc_per_pixel, nk_prop_color_flags flags, nk_color color, int forceOn = 0);
    NK_API int nk_propertyi(struct nk_context*, const char *name, int min, int val, int max, int step, float inc_per_pixel, nk_prop_color_flags flags, nk_color color);
    NK_API float nk_propertyf(struct nk_context*, const char *name, float min, float val, float max, float step, float inc_per_pixel, nk_prop_color_flags flags, nk_color color);
    NK_API double nk_propertyd(struct nk_context*, const char *name, double min, double val, double max, double step, float inc_per_pixel, nk_prop_color_flags flags, nk_color color);
    /* =============================================================================
    *
    *                                  TEXT EDIT
    *
    * ============================================================================= */
    enum nk_edit_flags {
        NK_EDIT_DEFAULT = 0,
        NK_EDIT_READ_ONLY = NK_FLAG(0),
        NK_EDIT_AUTO_SELECT = NK_FLAG(1),
        NK_EDIT_SIG_ENTER = NK_FLAG(2),
        NK_EDIT_ALLOW_TAB = NK_FLAG(3),
        NK_EDIT_NO_CURSOR = NK_FLAG(4),
        NK_EDIT_SELECTABLE = NK_FLAG(5),
        NK_EDIT_CLIPBOARD = NK_FLAG(6),
        NK_EDIT_CTRL_ENTER_NEWLINE = NK_FLAG(7),
        NK_EDIT_NO_HORIZONTAL_SCROLL = NK_FLAG(8),
        NK_EDIT_ALWAYS_INSERT_MODE = NK_FLAG(9),
        NK_EDIT_MULTILINE = NK_FLAG(10),
        NK_EDIT_GOTO_END_ON_ACTIVATE = NK_FLAG(11)
    };
    enum nk_edit_types {
        NK_EDIT_SIMPLE = NK_EDIT_ALWAYS_INSERT_MODE,
        NK_EDIT_FIELD = NK_EDIT_SIMPLE | NK_EDIT_SELECTABLE | NK_EDIT_CLIPBOARD,
        NK_EDIT_BOX = NK_EDIT_ALWAYS_INSERT_MODE | NK_EDIT_SELECTABLE | NK_EDIT_MULTILINE | NK_EDIT_ALLOW_TAB | NK_EDIT_CLIPBOARD,
        NK_EDIT_EDITOR = NK_EDIT_SELECTABLE | NK_EDIT_MULTILINE | NK_EDIT_ALLOW_TAB | NK_EDIT_CLIPBOARD
    };
    enum nk_edit_events {
        NK_EDIT_ACTIVE = NK_FLAG(0), /* edit widget is currently being modified */
        NK_EDIT_INACTIVE = NK_FLAG(1), /* edit widget is not active and is not being modified */
        NK_EDIT_ACTIVATED = NK_FLAG(2), /* edit widget went from state inactive to state active */
        NK_EDIT_DEACTIVATED = NK_FLAG(3), /* edit widget went from state active to state inactive */
        NK_EDIT_COMMITED = NK_FLAG(4) /* edit widget has received an enter and lost focus */
    };
    NK_API nk_flags nk_edit_string(struct nk_context*, nk_flags, char *buffer, int *len, int max, nk_plugin_filter);
    NK_API nk_flags nk_edit_string_zero_terminated(struct nk_context*, nk_flags, char *buffer, int max, nk_plugin_filter);
    NK_API nk_flags nk_edit_buffer(struct nk_context*, nk_flags, struct nk_text_edit*, nk_plugin_filter);
    NK_API void nk_edit_focus(struct nk_context*, nk_flags flags);
    NK_API void nk_edit_unfocus(struct nk_context*);
    /* =============================================================================
    *
    *                                  CHART
    *
    * ============================================================================= */
    NK_API int nk_chart_begin(struct nk_context*, enum nk_chart_type, int num, float min, float max);
    NK_API int nk_chart_begin_colored(struct nk_context*, enum nk_chart_type, struct nk_color, struct nk_color active, int num, float min, float max);
    NK_API void nk_chart_add_slot(struct nk_context *ctx, const enum nk_chart_type, int count, float min_value, float max_value);
    NK_API void nk_chart_add_slot_colored(struct nk_context *ctx, const enum nk_chart_type, struct nk_color, struct nk_color active, int count, float min_value, float max_value);
    NK_API nk_flags nk_chart_push(struct nk_context*, float);
    NK_API nk_flags nk_chart_push_slot(struct nk_context*, float, int);
    NK_API void nk_chart_end(struct nk_context*);
    NK_API void nk_plot(struct nk_context*, enum nk_chart_type, const float *values, int count, int offset);
    NK_API void nk_plot_function(struct nk_context*, enum nk_chart_type, void *userdata, float(*value_getter)(void* user, int index), int count, int offset);
    /* =============================================================================
    *
    *                                  POPUP
    *
    * ============================================================================= */
    NK_API int nk_popup_begin(struct nk_context*, enum nk_popup_type, const char*, nk_flags, struct nk_rect bounds);
    NK_API void nk_popup_close(struct nk_context*);
    NK_API void nk_popup_end(struct nk_context*);
    /* =============================================================================
    *
    *                                  COMBOBOX
    *
    * ============================================================================= */
    NK_API int nk_combo(struct nk_context*, const char **items, int count, int selected, int item_height, struct nk_vec2 size);
    NK_API int nk_combo_separator(struct nk_context*, const char *items_separated_by_separator, int separator, int selected, int count, int item_height, struct nk_vec2 size);
    NK_API int nk_combo_string(struct nk_context*, const char *items_separated_by_zeros, int selected, int count, int item_height, struct nk_vec2 size);
    NK_API int nk_combo_callback(struct nk_context*, void(*item_getter)(void*, int, const char**), void *userdata, int selected, int count, int item_height, struct nk_vec2 size);
    NK_API void nk_combobox(struct nk_context*, const char **items, int count, int *selected, int item_height, struct nk_vec2 size);
    NK_API void nk_combobox_string(struct nk_context*, const char *items_separated_by_zeros, int *selected, int count, int item_height, struct nk_vec2 size);
    NK_API void nk_combobox_separator(struct nk_context*, const char *items_separated_by_separator, int separator, int *selected, int count, int item_height, struct nk_vec2 size);
    NK_API void nk_combobox_callback(struct nk_context*, void(*item_getter)(void*, int, const char**), void*, int *selected, int count, int item_height, struct nk_vec2 size);
    /* =============================================================================
    *
    *                                  ABSTRACT COMBOBOX
    *
    * ============================================================================= */
    NK_API int nk_combo_begin_text(struct nk_context*, const char *selected, int, struct nk_vec2 size);
    NK_API int nk_combo_begin_label(struct nk_context*, const char *selected, struct nk_vec2 size);
    NK_API int nk_combo_begin_color(struct nk_context*, struct nk_color color, struct nk_vec2 size);
    NK_API int nk_combo_begin_symbol(struct nk_context*, enum nk_symbol_type, struct nk_vec2 size);
    NK_API int nk_combo_begin_symbol_label(struct nk_context*, const char *selected, enum nk_symbol_type, struct nk_vec2 size);
    NK_API int nk_combo_begin_symbol_text(struct nk_context*, const char *selected, int, enum nk_symbol_type, struct nk_vec2 size);
    NK_API int nk_combo_begin_image(struct nk_context*, struct nk_image img, struct nk_vec2 size);
    NK_API int nk_combo_begin_image_label(struct nk_context*, const char *selected, struct nk_image, struct nk_vec2 size);
    NK_API int nk_combo_begin_image_text(struct nk_context*, const char *selected, int, struct nk_image, struct nk_vec2 size);
    NK_API int nk_combo_item_label(struct nk_context*, const char*, nk_flags alignment);
    NK_API int nk_combo_item_text(struct nk_context*, const char*, int, nk_flags alignment);
    NK_API int nk_combo_item_image_label(struct nk_context*, struct nk_image, const char*, nk_flags alignment);
    NK_API int nk_combo_item_image_text(struct nk_context*, struct nk_image, const char*, int, nk_flags alignment);
    NK_API int nk_combo_item_symbol_label(struct nk_context*, enum nk_symbol_type, const char*, nk_flags alignment);
    NK_API int nk_combo_item_symbol_text(struct nk_context*, enum nk_symbol_type, const char*, int, nk_flags alignment);
    NK_API void nk_combo_close(struct nk_context*);
    NK_API void nk_combo_end(struct nk_context*);
    /* =============================================================================
    *
    *                                  CONTEXTUAL
    *
    * ============================================================================= */
    NK_API int nk_contextual_begin(struct nk_context*, nk_flags, struct nk_vec2, struct nk_rect trigger_bounds);
    NK_API int nk_contextual_item_text(struct nk_context*, const char*, int, nk_flags align);
    NK_API int nk_contextual_item_label(struct nk_context*, const char*, nk_flags align);
    NK_API int nk_contextual_item_image_label(struct nk_context*, struct nk_image, const char*, nk_flags alignment);
    NK_API int nk_contextual_item_image_text(struct nk_context*, struct nk_image, const char*, int len, nk_flags alignment);
    NK_API int nk_contextual_item_symbol_label(struct nk_context*, enum nk_symbol_type, const char*, nk_flags alignment);
    NK_API int nk_contextual_item_symbol_text(struct nk_context*, enum nk_symbol_type, const char*, int, nk_flags alignment);
    NK_API void nk_contextual_close(struct nk_context*);
    NK_API void nk_contextual_end(struct nk_context*);
    /* =============================================================================
    *
    *                                  TOOLTIP
    *
    * ============================================================================= */
    NK_API void nk_tooltip(struct nk_context*, const char*);
    NK_API int nk_tooltip_begin(struct nk_context*, float width);
    NK_API void nk_tooltip_end(struct nk_context*);
    /* =============================================================================
    *
    *                                  MENU
    *
    * ============================================================================= */
    NK_API void nk_menubar_begin(struct nk_context*);
    NK_API void nk_menubar_end(struct nk_context*);
    NK_API int nk_menu_begin_text(struct nk_context*, const char* title, int title_len, nk_flags align, struct nk_vec2 size);
    NK_API int nk_menu_begin_label(struct nk_context*, const char*, nk_flags align, struct nk_vec2 size);
    NK_API int nk_menu_begin_image(struct nk_context*, const char*, struct nk_image, struct nk_vec2 size);
    NK_API int nk_menu_begin_image_text(struct nk_context*, const char*, int, nk_flags align, struct nk_image, struct nk_vec2 size);
    NK_API int nk_menu_begin_image_label(struct nk_context*, const char*, nk_flags align, struct nk_image, struct nk_vec2 size);
    NK_API int nk_menu_begin_symbol(struct nk_context*, const char*, enum nk_symbol_type, struct nk_vec2 size);
    NK_API int nk_menu_begin_symbol_text(struct nk_context*, const char*, int, nk_flags align, enum nk_symbol_type, struct nk_vec2 size);
    NK_API int nk_menu_begin_symbol_label(struct nk_context*, const char*, nk_flags align, enum nk_symbol_type, struct nk_vec2 size);
    NK_API int nk_menu_item_text(struct nk_context*, const char*, int, nk_flags align);
    NK_API int nk_menu_item_label(struct nk_context*, const char*, nk_flags alignment);
    NK_API int nk_menu_item_image_label(struct nk_context*, struct nk_image, const char*, nk_flags alignment);
    NK_API int nk_menu_item_image_text(struct nk_context*, struct nk_image, const char*, int len, nk_flags alignment);
    NK_API int nk_menu_item_symbol_text(struct nk_context*, enum nk_symbol_type, const char*, int, nk_flags alignment);
    NK_API int nk_menu_item_symbol_label(struct nk_context*, enum nk_symbol_type, const char*, nk_flags alignment);
    NK_API void nk_menu_close(struct nk_context*);
    NK_API void nk_menu_end(struct nk_context*);
    /* =============================================================================
    *
    *                                  STYLE
    *
    * ============================================================================= */
    enum nk_style_colors {
        NK_COLOR_TEXT,
        NK_COLOR_WINDOW,
        NK_COLOR_HEADER,
        NK_COLOR_BORDER,
        NK_COLOR_BUTTON,
        NK_COLOR_BUTTON_HOVER,
        NK_COLOR_BUTTON_ACTIVE,
        NK_COLOR_TOGGLE,
        NK_COLOR_TOGGLE_HOVER,
        NK_COLOR_TOGGLE_CURSOR,
        NK_COLOR_SELECT,
        NK_COLOR_SELECT_ACTIVE,
        NK_COLOR_SLIDER,
        NK_COLOR_SLIDER_CURSOR,
        NK_COLOR_SLIDER_CURSOR_HOVER,
        NK_COLOR_SLIDER_CURSOR_ACTIVE,
        NK_COLOR_PROPERTY,
        NK_COLOR_EDIT,
        NK_COLOR_EDIT_CURSOR,
        NK_COLOR_COMBO,
        NK_COLOR_CHART,
        NK_COLOR_CHART_COLOR,
        NK_COLOR_CHART_COLOR_HIGHLIGHT,
        NK_COLOR_SCROLLBAR,
        NK_COLOR_SCROLLBAR_CURSOR,
        NK_COLOR_SCROLLBAR_CURSOR_HOVER,
        NK_COLOR_SCROLLBAR_CURSOR_ACTIVE,
        NK_COLOR_TAB_HEADER,
        NK_COLOR_COUNT
    };
    enum nk_style_cursor {
        NK_CURSOR_ARROW,
        NK_CURSOR_TEXT,
        NK_CURSOR_MOVE,
        NK_CURSOR_RESIZE_VERTICAL,
        NK_CURSOR_RESIZE_HORIZONTAL,
        NK_CURSOR_RESIZE_TOP_LEFT_DOWN_RIGHT,
        NK_CURSOR_RESIZE_TOP_RIGHT_DOWN_LEFT,
        NK_CURSOR_COUNT
    };
    NK_API void nk_style_default(struct nk_context*);
    NK_API void nk_style_from_table(struct nk_context*, const struct nk_color*);
    NK_API void nk_style_load_cursor(struct nk_context*, enum nk_style_cursor, const struct nk_cursor*);
    NK_API void nk_style_load_all_cursors(struct nk_context*, struct nk_cursor*);
    NK_API const char* nk_style_get_color_by_name(enum nk_style_colors);
    NK_API void nk_style_set_font(struct nk_context*, const struct nk_user_font*);
    NK_API int nk_style_set_cursor(struct nk_context*, enum nk_style_cursor);
    NK_API void nk_style_show_cursor(struct nk_context*);
    NK_API void nk_style_hide_cursor(struct nk_context*);

    NK_API int nk_style_push_font(struct nk_context*, struct nk_user_font*);
    NK_API int nk_style_push_float(struct nk_context*, float*, float);
    NK_API int nk_style_push_vec2(struct nk_context*, struct nk_vec2*, struct nk_vec2);
    NK_API int nk_style_push_style_item(struct nk_context*, struct nk_style_item*, struct nk_style_item);
    NK_API int nk_style_push_flags(struct nk_context*, nk_flags*, nk_flags);
    NK_API int nk_style_push_color(struct nk_context*, struct nk_color*, struct nk_color);

    NK_API int nk_style_pop_font(struct nk_context*);
    NK_API int nk_style_pop_float(struct nk_context*);
    NK_API int nk_style_pop_vec2(struct nk_context*);
    NK_API int nk_style_pop_style_item(struct nk_context*);
    NK_API int nk_style_pop_flags(struct nk_context*);
    NK_API int nk_style_pop_color(struct nk_context*);
    /* =============================================================================
    *
    *                                  COLOR
    *
    * ============================================================================= */
    NK_API struct nk_color nk_rgb(int r, int g, int b);
    NK_API struct nk_color nk_rgb_iv(const int *rgb);
    NK_API struct nk_color nk_rgb_bv(const nk_byte* rgb);
    NK_API struct nk_color nk_rgb_f(float r, float g, float b);
    NK_API struct nk_color nk_rgb_fv(const float *rgb);
    NK_API struct nk_color nk_rgb_hex(const char *rgb);

    NK_API struct nk_color nk_rgba(int r, int g, int b, int a);
    NK_API struct nk_color nk_rgba_u32(nk_uint);
    NK_API struct nk_color nk_rgba_iv(const int *rgba);
    NK_API struct nk_color nk_rgba_bv(const nk_byte *rgba);
    NK_API struct nk_color nk_rgba_f(float r, float g, float b, float a);
    NK_API struct nk_color nk_rgba_fv(const float *rgba);
    NK_API struct nk_color nk_rgba_hex(const char *rgb);

    NK_API struct nk_color nk_hsv(int h, int s, int v);
    NK_API struct nk_color nk_hsv_iv(const int *hsv);
    NK_API struct nk_color nk_hsv_bv(const nk_byte *hsv);
    NK_API struct nk_color nk_hsv_f(float h, float s, float v);
    NK_API struct nk_color nk_hsv_fv(const float *hsv);

    NK_API struct nk_color nk_hsva(int h, int s, int v, int a);
    NK_API struct nk_color nk_hsva_iv(const int *hsva);
    NK_API struct nk_color nk_hsva_bv(const nk_byte *hsva);
    NK_API struct nk_color nk_hsva_f(float h, float s, float v, float a);
    NK_API struct nk_color nk_hsva_fv(const float *hsva);

    /* color (conversion nuklear --> user) */
    NK_API void nk_color_f(float *r, float *g, float *b, float *a, struct nk_color);
    NK_API void nk_color_fv(float *rgba_out, struct nk_color);
    NK_API void nk_color_d(double *r, double *g, double *b, double *a, struct nk_color);
    NK_API void nk_color_dv(double *rgba_out, struct nk_color);

    NK_API nk_uint nk_color_u32(struct nk_color);
    NK_API void nk_color_hex_rgba(char *output, struct nk_color);
    NK_API void nk_color_hex_rgb(char *output, struct nk_color);

    NK_API void nk_color_hsv_i(int *out_h, int *out_s, int *out_v, struct nk_color);
    NK_API void nk_color_hsv_b(nk_byte *out_h, nk_byte *out_s, nk_byte *out_v, struct nk_color);
    NK_API void nk_color_hsv_iv(int *hsv_out, struct nk_color);
    NK_API void nk_color_hsv_bv(nk_byte *hsv_out, struct nk_color);
    NK_API void nk_color_hsv_f(float *out_h, float *out_s, float *out_v, struct nk_color);
    NK_API void nk_color_hsv_fv(float *hsv_out, struct nk_color);

    NK_API void nk_color_hsva_i(int *h, int *s, int *v, int *a, struct nk_color);
    NK_API void nk_color_hsva_b(nk_byte *h, nk_byte *s, nk_byte *v, nk_byte *a, struct nk_color);
    NK_API void nk_color_hsva_iv(int *hsva_out, struct nk_color);
    NK_API void nk_color_hsva_bv(nk_byte *hsva_out, struct nk_color);
    NK_API void nk_color_hsva_f(float *out_h, float *out_s, float *out_v, float *out_a, struct nk_color);
    NK_API void nk_color_hsva_fv(float *hsva_out, struct nk_color);
    /* =============================================================================
    *
    *                                  IMAGE
    *
    * ============================================================================= */
    NK_API nk_handle nk_handle_ptr(void*);
    NK_API nk_handle nk_handle_id(int);
    NK_API struct nk_image nk_image_handle(nk_handle);
    NK_API struct nk_image nk_image_ptr(void*);
    NK_API struct nk_image nk_image_id(int);
    NK_API int nk_image_is_subimage(const struct nk_image* img);
    NK_API struct nk_image nk_subimage_ptr(void*, unsigned short w, unsigned short h, struct nk_rect sub_region);
    NK_API struct nk_image nk_subimage_id(int, unsigned short w, unsigned short h, struct nk_rect sub_region);
    NK_API struct nk_image nk_subimage_handle(nk_handle, unsigned short w, unsigned short h, struct nk_rect sub_region);
    /* =============================================================================
    *
    *                                  MATH
    *
    * ============================================================================= */
    NK_API nk_hash nk_murmur_hash(const void *key, int len, nk_hash seed);
    NK_API void nk_triangle_from_direction(struct nk_vec2 *result, struct nk_rect r, float pad_x, float pad_y, enum nk_heading);

    NK_API struct nk_vec2 nk_vec2(float x, float y);
    NK_API struct nk_vec2 nk_vec2i(int x, int y);
    NK_API struct nk_vec2 nk_vec2v(const float *xy);
    NK_API struct nk_vec2 nk_vec2iv(const int *xy);

    NK_API struct nk_rect nk_get_null_rect(void);
    NK_API struct nk_rect nk_rect(float x, float y, float w, float h);
    NK_API struct nk_rect nk_recti(int x, int y, int w, int h);
    NK_API struct nk_rect nk_recta(struct nk_vec2 pos, struct nk_vec2 size);
    NK_API struct nk_rect nk_rectv(const float *xywh);
    NK_API struct nk_rect nk_rectiv(const int *xywh);
    NK_API struct nk_vec2 nk_rect_pos(struct nk_rect);
    NK_API struct nk_vec2 nk_rect_size(struct nk_rect);
    /* =============================================================================
    *
    *                                  STRING
    *
    * ============================================================================= */
    NK_API int nk_strlen(const char *str);
    NK_API int nk_stricmp(const char *s1, const char *s2);
    NK_API int nk_stricmpn(const char *s1, const char *s2, int n);
    NK_API int nk_strtoi(const char *str, const char **endptr);
    NK_API float nk_strtof(const char *str, const char **endptr);
    NK_API double nk_strtod(const char *str, const char **endptr);
    NK_API int nk_strfilter(const char *text, const char *regexp);
    NK_API int nk_strmatch_fuzzy_string(char const *str, char const *pattern, int *out_score);
    NK_API int nk_strmatch_fuzzy_text(const char *txt, int txt_len, const char *pattern, int *out_score);
    /* =============================================================================
    *
    *                                  UTF-8
    *
    * ============================================================================= */
    NK_API int nk_utf_decode(const char*, nk_rune*, int);
    NK_API int nk_utf_encode(nk_rune, char*, int);
    NK_API int nk_utf_len(const char*, int byte_len);
    NK_API const char* nk_utf_at(const char *buffer, int length, int index, nk_rune *unicode, int *len);
    /* ===============================================================
    *
    *                          FONT
    *
    * ===============================================================*/
    /*  Font handling in this library was designed to be quite customizable and lets
    you decide what you want to use and what you want to provide. There are three
    different ways to use the font atlas. The first two will use your font
    handling scheme and only requires essential data to run nuklear. The next
    slightly more advanced features is font handling with vertex buffer output.
    Finally the most complex API wise is using nuklears font baking API.

    1.) Using your own implementation without vertex buffer output
    --------------------------------------------------------------
    So first up the easiest way to do font handling is by just providing a
    `nk_user_font` struct which only requires the height in pixel of the used
    font and a callback to calculate the width of a string. This way of handling
    fonts is best fitted for using the normal draw shape command API where you
    do all the text drawing yourself and the library does not require any kind
    of deeper knowledge about which font handling mechanism you use.
    IMPORTANT: the `nk_user_font` pointer provided to nuklear has to persist
    over the complete life time! I know this sucks but it is currently the only
    way to switch between fonts.

    float your_text_width_calculation(nk_handle handle, float height, const char *text, int len)
    {
    your_font_type *type = handle.ptr;
    float text_width = ...;
    return text_width;
    }

    struct nk_user_font font;
    font.userdata.ptr = &your_font_class_or_struct;
    font.height = your_font_height;
    font.width = your_text_width_calculation;

    struct nk_context ctx;
    nk_init_default(&ctx, &font);

    2.) Using your own implementation with vertex buffer output
    --------------------------------------------------------------
    While the first approach works fine if you don't want to use the optional
    vertex buffer output it is not enough if you do. To get font handling working
    for these cases you have to provide two additional parameters inside the
    `nk_user_font`. First a texture atlas handle used to draw text as subimages
    of a bigger font atlas texture and a callback to query a character's glyph
    information (offset, size, ...). So it is still possible to provide your own
    font and use the vertex buffer output.

    float your_text_width_calculation(nk_handle handle, float height, const char *text, int len)
    {
    your_font_type *type = handle.ptr;
    float text_width = ...;
    return text_width;
    }
    void query_your_font_glyph(nk_handle handle, float font_height, struct nk_user_font_glyph *glyph, nk_rune codepoint, nk_rune next_codepoint)
    {
    your_font_type *type = handle.ptr;
    glyph.width = ...;
    glyph.height = ...;
    glyph.xadvance = ...;
    glyph.uv[0].x = ...;
    glyph.uv[0].y = ...;
    glyph.uv[1].x = ...;
    glyph.uv[1].y = ...;
    glyph.offset.x = ...;
    glyph.offset.y = ...;
    }

    struct nk_user_font font;
    font.userdata.ptr = &your_font_class_or_struct;
    font.height = your_font_height;
    font.width = your_text_width_calculation;
    font.query = query_your_font_glyph;
    font.texture.id = your_font_texture;

    struct nk_context ctx;
    nk_init_default(&ctx, &font);

    3.) Nuklear font baker
    ------------------------------------
    The final approach if you do not have a font handling functionality or don't
    want to use it in this library is by using the optional font baker.
    The font baker API's can be used to create a font plus font atlas texture
    and can be used with or without the vertex buffer output.

    It still uses the `nk_user_font` struct and the two different approaches
    previously stated still work. The font baker is not located inside
    `nk_context` like all other systems since it can be understood as more of
    an extension to nuklear and does not really depend on any `nk_context` state.

    Font baker need to be initialized first by one of the nk_font_atlas_init_xxx
    functions. If you don't care about memory just call the default version
    `nk_font_atlas_init_default` which will allocate all memory from the standard library.
    If you want to control memory allocation but you don't care if the allocated
    memory is temporary and therefore can be freed directly after the baking process
    is over or permanent you can call `nk_font_atlas_init`.

    After successfull intializing the font baker you can add Truetype(.ttf) fonts from
    different sources like memory or from file by calling one of the `nk_font_atlas_add_xxx`.
    functions. Adding font will permanently store each font, font config and ttf memory block(!)
    inside the font atlas and allows to reuse the font atlas. If you don't want to reuse
    the font baker by for example adding additional fonts you can call
    `nk_font_atlas_cleanup` after the baking process is over (after calling nk_font_atlas_end).

    As soon as you added all fonts you wanted you can now start the baking process
    for every selected glyphes to image by calling `nk_font_atlas_bake`.
    The baking process returns image memory, width and height which can be used to
    either create your own image object or upload it to any graphics library.
    No matter which case you finally have to call `nk_font_atlas_end` which
    will free all temporary memory including the font atlas image so make sure
    you created our texture beforehand. `nk_font_atlas_end` requires a handle
    to your font texture or object and optionally fills a `struct nk_draw_null_texture`
    which can be used for the optional vertex output. If you don't want it just
    set the argument to `NULL`.

    At this point you are done and if you don't want to reuse the font atlas you
    can call `nk_font_atlas_cleanup` to free all truetype blobs and configuration
    memory. Finally if you don't use the font atlas and any of it's fonts anymore
    you need to call `nk_font_atlas_clear` to free all memory still being used.

    struct nk_font_atlas atlas;
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
    nk_font *font = nk_font_atlas_add_from_file(&atlas, "Path/To/Your/TTF_Font.ttf", 13, 0);
    nk_font *font2 = nk_font_atlas_add_from_file(&atlas, "Path/To/Your/TTF_Font2.ttf", 16, 0);
    const void* img = nk_font_atlas_bake(&atlas, &img_width, &img_height, NK_FONT_ATLAS_RGBA32);
    nk_font_atlas_end(&atlas, nk_handle_id(texture), 0);

    struct nk_context ctx;
    nk_init_default(&ctx, &font->handle);
    while (1) {

    }
    nk_font_atlas_clear(&atlas);

    The font baker API is probably the most complex API inside this library and
    I would suggest reading some of my examples `example/` to get a grip on how
    to use the font atlas. There are a number of details I left out. For example
    how to merge fonts, configure a font with `nk_font_config` to use other languages,
    use another texture coodinate format and a lot more:

    struct nk_font_config cfg = nk_font_config(font_pixel_height);
    cfg.merge_mode = nk_false or nk_true;
    cfg.range = nk_font_korean_glyph_ranges();
    cfg.coord_type = NK_COORD_PIXEL;
    nk_font *font = nk_font_atlas_add_from_file(&atlas, "Path/To/Your/TTF_Font.ttf", 13, &cfg);

    */
    struct nk_user_font_glyph;
    typedef float(*nk_text_width_f)(nk_handle, float h, const char*, int len);
    typedef void(*nk_query_font_glyph_f)(nk_handle handle, float font_height,
        struct nk_user_font_glyph *glyph,
        nk_rune codepoint, nk_rune next_codepoint);

    typedef void(*nk_hotspot_output)(nk_handle, struct nk_hotspot);

#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
    struct nk_user_font_glyph {
        struct nk_vec2 uv[2];
        /* texture coordinates */
        struct nk_vec2 offset;
        /* offset between top left and glyph */
        float width, height;
        /* size of the glyph  */
        float xadvance;
        /* offset to the next glyph */
    };
#endif

    struct nk_user_font {
        nk_handle userdata;
        /* user provided font handle */
        float height;
        /* max height of the font */
        nk_text_width_f width;
        /* font string width in pixel callback */
#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
        nk_query_font_glyph_f query;
        /* font glyph callback to query drawing info */
        nk_handle texture;
        /* texture handle to the used font atlas or texture */
#endif
    };

#ifdef NK_INCLUDE_FONT_BAKING
    enum nk_font_coord_type {
        NK_COORD_UV, /* texture coordinates inside font glyphs are clamped between 0-1 */
        NK_COORD_PIXEL /* texture coordinates inside font glyphs are in absolute pixel */
    };

    struct nk_baked_font {
        float height;
        /* height of the font  */
        float ascent, descent;
        /* font glyphs ascent and descent  */
        nk_rune glyph_offset;
        /* glyph array offset inside the font glyph baking output array  */
        nk_rune glyph_count;
        /* number of glyphs of this font inside the glyph baking array output */
        const nk_rune *ranges;
        /* font codepoint ranges as pairs of (from/to) and 0 as last element */
    };

    struct nk_font_config {
        struct nk_font_config *next;
        /* NOTE: only used internally */
        void *ttf_blob;
        /* pointer to loaded TTF file memory block.
        * NOTE: not needed for nk_font_atlas_add_from_memory and nk_font_atlas_add_from_file. */
        nk_size ttf_size;
        /* size of the loaded TTF file memory block
        * NOTE: not needed for nk_font_atlas_add_from_memory and nk_font_atlas_add_from_file. */

        unsigned char ttf_data_owned_by_atlas;
        /* used inside font atlas: default to: 0*/
        unsigned char merge_mode;
        /* merges this font into the last font */
        unsigned char pixel_snap;
        /* align every character to pixel boundary (if true set oversample (1,1)) */
        unsigned char oversample_v, oversample_h;
        /* rasterize at hight quality for sub-pixel position */
        unsigned char padding[3];

        float size;
        /* baked pixel height of the font */
        enum nk_font_coord_type coord_type;
        /* texture coordinate format with either pixel or UV coordinates */
        struct nk_vec2 spacing;
        /* extra pixel spacing between glyphs  */
        const nk_rune *range;
        /* list of unicode ranges (2 values per range, zero terminated) */
        struct nk_baked_font *font;
        /* font to setup in the baking process: NOTE: not needed for font atlas */
        nk_rune fallback_glyph;
        /* fallback glyph to use if a given rune is not found */
    };

    struct nk_font_glyph {
        nk_rune codepoint;
        float xadvance;
        float x0, y0, x1, y1, w, h;
        float u0, v0, u1, v1;
    };

    struct nk_font {
        struct nk_font *next;
        struct nk_user_font handle;
        struct nk_baked_font info;
        float scale;
        struct nk_font_glyph *glyphs;
        const struct nk_font_glyph *fallback;
        nk_rune fallback_codepoint;
        nk_handle texture;
        struct nk_font_config *config;
    };

    enum nk_font_atlas_format {
        NK_FONT_ATLAS_ALPHA8,
        NK_FONT_ATLAS_RGBA32
    };

    struct nk_font_atlas {
        void *pixel;
        int tex_width;
        int tex_height;

        struct nk_allocator permanent;
        struct nk_allocator temporary;

        struct nk_recti custom;
        struct nk_cursor cursors[NK_CURSOR_COUNT];

        int glyph_count;
        struct nk_font_glyph *glyphs;
        struct nk_font *default_font;
        struct nk_font *fonts;
        struct nk_font_config *config;
        int font_num;
    };

    /* some language glyph codepoint ranges */
    NK_API const nk_rune *nk_font_default_glyph_ranges(void);
    NK_API const nk_rune *nk_font_chinese_glyph_ranges(void);
    NK_API const nk_rune *nk_font_cyrillic_glyph_ranges(void);
    NK_API const nk_rune *nk_font_korean_glyph_ranges(void);

#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
    NK_API void nk_font_atlas_init_default(struct nk_font_atlas*);
#endif
    NK_API void nk_font_atlas_init(struct nk_font_atlas*, struct nk_allocator*);
    NK_API void nk_font_atlas_init_custom(struct nk_font_atlas*, struct nk_allocator *persistent, struct nk_allocator *transient);
    NK_API void nk_font_atlas_begin(struct nk_font_atlas*);
    NK_API struct nk_font_config nk_font_config(float pixel_height);
    NK_API struct nk_font *nk_font_atlas_add(struct nk_font_atlas*, const struct nk_font_config*);
#ifdef NK_INCLUDE_DEFAULT_FONT
    NK_API struct nk_font* nk_font_atlas_add_default(struct nk_font_atlas*, float height, const struct nk_font_config*);
#endif
    NK_API struct nk_font* nk_font_atlas_add_from_memory(struct nk_font_atlas *atlas, void *memory, nk_size size, float height, const struct nk_font_config *config);
#ifdef NK_INCLUDE_STANDARD_IO
    NK_API struct nk_font* nk_font_atlas_add_from_file(struct nk_font_atlas *atlas, const char *file_path, float height, const struct nk_font_config*);
#endif
    NK_API struct nk_font *nk_font_atlas_add_compressed(struct nk_font_atlas*, void *memory, nk_size size, float height, const struct nk_font_config*);
    NK_API struct nk_font* nk_font_atlas_add_compressed_base85(struct nk_font_atlas*, const char *data, float height, const struct nk_font_config *config);
    NK_API const void* nk_font_atlas_bake(struct nk_font_atlas*, int *width, int *height, enum nk_font_atlas_format);
    NK_API void nk_font_atlas_end(struct nk_font_atlas*, nk_handle tex, struct nk_draw_null_texture*);
    NK_API const struct nk_font_glyph* nk_font_find_glyph(struct nk_font*, nk_rune unicode);
    NK_API void nk_font_atlas_cleanup(struct nk_font_atlas *atlas);
    NK_API void nk_font_atlas_clear(struct nk_font_atlas*);

#endif

    /* ==============================================================
    *
    *                          MEMORY BUFFER
    *
    * ===============================================================*/
    /*  A basic (double)-buffer with linear allocation and resetting as only
    freeing policy. The buffer's main purpose is to control all memory management
    inside the GUI toolkit and still leave memory control as much as possible in
    the hand of the user while also making sure the library is easy to use if
    not as much control is needed.
    In general all memory inside this library can be provided from the user in
    three different ways.

    The first way and the one providing most control is by just passing a fixed
    size memory block. In this case all control lies in the hand of the user
    since he can exactly control where the memory comes from and how much memory
    the library should consume. Of course using the fixed size API removes the
    ability to automatically resize a buffer if not enough memory is provided so
    you have to take over the resizing. While being a fixed sized buffer sounds
    quite limiting, it is very effective in this library since the actual memory
    consumption is quite stable and has a fixed upper bound for a lot of cases.

    If you don't want to think about how much memory the library should allocate
    at all time or have a very dynamic UI with unpredictable memory consumption
    habits but still want control over memory allocation you can use the dynamic
    allocator based API. The allocator consists of two callbacks for allocating
    and freeing memory and optional userdata so you can plugin your own allocator.

    The final and easiest way can be used by defining
    NK_INCLUDE_DEFAULT_ALLOCATOR which uses the standard library memory
    allocation functions malloc and free and takes over complete control over
    memory in this library.
    */
    struct nk_memory_status {
        void *memory;
        unsigned int type;
        nk_size size;
        nk_size allocated;
        nk_size needed;
        nk_size calls;
    };

    enum nk_allocation_type {
        NK_BUFFER_FIXED,
        NK_BUFFER_DYNAMIC
    };

    enum nk_buffer_allocation_type {
        NK_BUFFER_FRONT,
        NK_BUFFER_BACK,
        NK_BUFFER_MAX
    };

    struct nk_buffer_marker {
        int active;
        nk_size offset;
    };

    struct nk_memory { void *ptr; nk_size size; };
    struct nk_buffer {
        struct nk_buffer_marker marker[NK_BUFFER_MAX];
        /* buffer marker to free a buffer to a certain offset */
        struct nk_allocator pool;
        /* allocator callback for dynamic buffers */
        enum nk_allocation_type type;
        /* memory management type */
        struct nk_memory memory;
        /* memory and size of the current memory block */
        float grow_factor;
        /* growing factor for dynamic memory management */
        nk_size allocated;
        /* total amount of memory allocated */
        nk_size needed;
        /* totally consumed memory given that enough memory is present */
        nk_size calls;
        /* number of allocation calls */
        nk_size size;
        /* current size of the buffer */
    };

#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
    NK_API void nk_buffer_init_default(struct nk_buffer*);
#endif
    NK_API void nk_buffer_init(struct nk_buffer*, const struct nk_allocator*, nk_size size);
    NK_API void nk_buffer_init_fixed(struct nk_buffer*, void *memory, nk_size size);
    NK_API void nk_buffer_info(struct nk_memory_status*, struct nk_buffer*);
    NK_API void nk_buffer_push(struct nk_buffer*, enum nk_buffer_allocation_type type, const void *memory, nk_size size, nk_size align);
    NK_API void nk_buffer_mark(struct nk_buffer*, enum nk_buffer_allocation_type type);
    NK_API void nk_buffer_reset(struct nk_buffer*, enum nk_buffer_allocation_type type);
    NK_API void nk_buffer_clear(struct nk_buffer*);
    NK_API void nk_buffer_free(struct nk_buffer*);
    NK_API void *nk_buffer_memory(struct nk_buffer*);
    NK_API const void *nk_buffer_memory_const(const struct nk_buffer*);
    NK_API nk_size nk_buffer_total(struct nk_buffer*);

    /* ==============================================================
    *
    *                          STRING
    *
    * ===============================================================*/
    /*  Basic string buffer which is only used in context with the text editor
    *  to manage and manipulate dynamic or fixed size string content. This is _NOT_
    *  the default string handling method. The only instance you should have any contact
    *  with this API is if you interact with an `nk_text_edit` object inside one of the
    *  copy and paste functions and even there only for more advanced cases. */
    struct nk_str {
        struct nk_buffer buffer;
        int len; /* in codepoints/runes/glyphs */
    };

#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
    NK_API void nk_str_init_default(struct nk_str*);
#endif
    NK_API void nk_str_init(struct nk_str*, const struct nk_allocator*, nk_size size);
    NK_API void nk_str_init_fixed(struct nk_str*, void *memory, nk_size size);
    NK_API void nk_str_clear(struct nk_str*);
    NK_API void nk_str_free(struct nk_str*);

    NK_API int nk_str_append_text_char(struct nk_str*, const char*, int);
    NK_API int nk_str_append_str_char(struct nk_str*, const char*);
    NK_API int nk_str_append_text_utf8(struct nk_str*, const char*, int);
    NK_API int nk_str_append_str_utf8(struct nk_str*, const char*);
    NK_API int nk_str_append_text_runes(struct nk_str*, const nk_rune*, int);
    NK_API int nk_str_append_str_runes(struct nk_str*, const nk_rune*);

    NK_API int nk_str_insert_at_char(struct nk_str*, int pos, const char*, int);
    NK_API int nk_str_insert_at_rune(struct nk_str*, int pos, const char*, int);

    NK_API int nk_str_insert_text_char(struct nk_str*, int pos, const char*, int);
    NK_API int nk_str_insert_str_char(struct nk_str*, int pos, const char*);
    NK_API int nk_str_insert_text_utf8(struct nk_str*, int pos, const char*, int);
    NK_API int nk_str_insert_str_utf8(struct nk_str*, int pos, const char*);
    NK_API int nk_str_insert_text_runes(struct nk_str*, int pos, const nk_rune*, int);
    NK_API int nk_str_insert_str_runes(struct nk_str*, int pos, const nk_rune*);

    NK_API void nk_str_remove_chars(struct nk_str*, int len);
    NK_API void nk_str_remove_runes(struct nk_str *str, int len);
    NK_API void nk_str_delete_chars(struct nk_str*, int pos, int len);
    NK_API void nk_str_delete_runes(struct nk_str*, int pos, int len);

    NK_API char *nk_str_at_char(struct nk_str*, int pos);
    NK_API char *nk_str_at_rune(struct nk_str*, int pos, nk_rune *unicode, int *len);
    NK_API nk_rune nk_str_rune_at(const struct nk_str*, int pos);
    NK_API const char *nk_str_at_char_const(const struct nk_str*, int pos);
    NK_API const char *nk_str_at_const(const struct nk_str*, int pos, nk_rune *unicode, int *len);

    NK_API char *nk_str_get(struct nk_str*);
    NK_API const char *nk_str_get_const(const struct nk_str*);
    NK_API int nk_str_len(struct nk_str*);
    NK_API int nk_str_len_char(struct nk_str*);

    /*===============================================================
    *
    *                      TEXT EDITOR
    *
    * ===============================================================*/
    /* Editing text in this library is handled by either `nk_edit_string` or
    * `nk_edit_buffer`. But like almost everything in this library there are multiple
    * ways of doing it and a balance between control and ease of use with memory
    * as well as functionality controlled by flags.
    *
    * This library generally allows three different levels of memory control:
    * First of is the most basic way of just providing a simple char array with
    * string length. This method is probably the easiest way of handling simple
    * user text input. Main upside is complete control over memory while the biggest
    * downside in comparsion with the other two approaches is missing undo/redo.
    *
    * For UIs that require undo/redo the second way was created. It is based on
    * a fixed size nk_text_edit struct, which has an internal undo/redo stack.
    * This is mainly useful if you want something more like a text editor but don't want
    * to have a dynamically growing buffer.
    *
    * The final way is using a dynamically growing nk_text_edit struct, which
    * has both a default version if you don't care where memory comes from and an
    * allocator version if you do. While the text editor is quite powerful for its
    * complexity I would not recommend editing gigabytes of data with it.
    * It is rather designed for uses cases which make sense for a GUI library not for
    * an full blown text editor.
    */
#ifndef NK_TEXTEDIT_UNDOSTATECOUNT
#define NK_TEXTEDIT_UNDOSTATECOUNT     99
#endif

#ifndef NK_TEXTEDIT_UNDOCHARCOUNT
#define NK_TEXTEDIT_UNDOCHARCOUNT      999
#endif

    struct nk_text_edit;
    struct nk_clipboard {
        nk_handle userdata;
        nk_plugin_paste paste;
        nk_plugin_copy copy;
    };

    struct nk_text_undo_record {
        int where;
        short insert_length;
        short delete_length;
        short char_storage;
    };

    struct nk_text_undo_state {
        struct nk_text_undo_record undo_rec[NK_TEXTEDIT_UNDOSTATECOUNT];
        nk_rune undo_char[NK_TEXTEDIT_UNDOCHARCOUNT];
        short undo_point;
        short redo_point;
        short undo_char_point;
        short redo_char_point;
    };

    enum nk_text_edit_type {
        NK_TEXT_EDIT_SINGLE_LINE,
        NK_TEXT_EDIT_MULTI_LINE
    };

    enum nk_text_edit_mode {
        NK_TEXT_EDIT_MODE_VIEW,
        NK_TEXT_EDIT_MODE_INSERT,
        NK_TEXT_EDIT_MODE_REPLACE
    };

    struct nk_text_edit {
        struct nk_clipboard clip;
        struct nk_str string;
        nk_plugin_filter filter;
        struct nk_vec2 scrollbar;

        int cursor;
        int select_start;
        int select_end;
        unsigned char mode;
        unsigned char cursor_at_end_of_line;
        unsigned char initialized;
        unsigned char has_preferred_x;
        unsigned char single_line;
        unsigned char active;
        unsigned char padding1;
        float preferred_x;
        struct nk_text_undo_state undo;
    };

    /* filter function */
    NK_API int nk_filter_default(const struct nk_text_edit*, nk_rune unicode);
    NK_API int nk_filter_ascii(const struct nk_text_edit*, nk_rune unicode);
    NK_API int nk_filter_float(const struct nk_text_edit*, nk_rune unicode);
    NK_API int nk_filter_decimal(const struct nk_text_edit*, nk_rune unicode);
    NK_API int nk_filter_hex(const struct nk_text_edit*, nk_rune unicode);
    NK_API int nk_filter_oct(const struct nk_text_edit*, nk_rune unicode);
    NK_API int nk_filter_binary(const struct nk_text_edit*, nk_rune unicode);

    /* text editor */
#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
    NK_API void nk_textedit_init_default(struct nk_text_edit*);
#endif
    NK_API void nk_textedit_init(struct nk_text_edit*, struct nk_allocator*, nk_size size);
    NK_API void nk_textedit_init_fixed(struct nk_text_edit*, void *memory, nk_size size);
    NK_API void nk_textedit_free(struct nk_text_edit*);
    NK_API void nk_textedit_text(struct nk_text_edit*, const char*, int total_len);
    NK_API void nk_textedit_delete(struct nk_text_edit*, int where, int len);
    NK_API void nk_textedit_delete_selection(struct nk_text_edit*);
    NK_API void nk_textedit_select_all(struct nk_text_edit*);
    NK_API int nk_textedit_cut(struct nk_text_edit*);
    NK_API int nk_textedit_paste(struct nk_text_edit*, char const*, int len);
    NK_API void nk_textedit_undo(struct nk_text_edit*);
    NK_API void nk_textedit_redo(struct nk_text_edit*);

    /* ===============================================================
    *
    *                          DRAWING
    *
    * ===============================================================*/
    /*  This library was designed to be render backend agnostic so it does
    not draw anything to screen. Instead all drawn shapes, widgets
    are made of, are buffered into memory and make up a command queue.
    Each frame therefore fills the command buffer with draw commands
    that then need to be executed by the user and his own render backend.
    After that the command buffer needs to be cleared and a new frame can be
    started. It is probably important to note that the command buffer is the main
    drawing API and the optional vertex buffer API only takes this format and
    converts it into a hardware accessible format.

    To use the command queue to draw your own widgets you can access the
    command buffer of each window by calling `nk_window_get_canvas` after
    previously having called `nk_begin`:

    void draw_red_rectangle_widget(struct nk_context *ctx)
    {
    struct nk_command_buffer *canvas;
    struct nk_input *input = &ctx->input;
    canvas = nk_window_get_canvas(ctx);

    struct nk_rect space;
    enum nk_widget_layout_states state;
    state = nk_widget(&space, ctx);
    if (!state) return;

    if (state != NK_WIDGET_ROM)
    update_your_widget_by_user_input(...);
    nk_fill_rect(canvas, space, 0, nk_rgb(255,0,0));
    }

    if (nk_begin(...)) {
    nk_layout_row_dynamic(ctx, 25, 1);
    draw_red_rectangle_widget(ctx);
    }
    nk_end(..)

    Important to know if you want to create your own widgets is the `nk_widget`
    call. It allocates space on the panel reserved for this widget to be used,
    but also returns the state of the widget space. If your widget is not seen and does
    not have to be updated it is '0' and you can just return. If it only has
    to be drawn the state will be `NK_WIDGET_ROM` otherwise you can do both
    update and draw your widget. The reason for seperating is to only draw and
    update what is actually neccessary which is crucial for performance.
    */
    enum nk_command_type {
        NK_COMMAND_NOP,
        NK_COMMAND_SCISSOR,
        NK_COMMAND_LINE,
        NK_COMMAND_CURVE,
        NK_COMMAND_RECT,
        NK_COMMAND_RECT_FILLED,
        NK_COMMAND_RECT_MULTI_COLOR,
        NK_COMMAND_CIRCLE,
        NK_COMMAND_CIRCLE_FILLED,
        NK_COMMAND_ARC,
        NK_COMMAND_ARC_FILLED,
        NK_COMMAND_TRIANGLE,
        NK_COMMAND_TRIANGLE_FILLED,
        NK_COMMAND_POLYGON,
        NK_COMMAND_POLYGON_FILLED,
        NK_COMMAND_POLYLINE,
        NK_COMMAND_TEXT,
        NK_COMMAND_IMAGE,
        NK_COMMAND_CUSTOM
    };

    /* command base and header of every command inside the buffer */
    struct nk_command {
        enum nk_command_type type;
        nk_size next;
#ifdef NK_INCLUDE_COMMAND_USERDATA
        nk_handle userdata;
#endif
    };

    struct nk_command_scissor {
        struct nk_command header;
        short x, y;
        unsigned short w, h;
    };

    struct nk_command_line {
        struct nk_command header;
        unsigned short line_thickness;
        struct nk_vec2i begin;
        struct nk_vec2i end;
        struct nk_color color;
    };

    struct nk_command_curve {
        struct nk_command header;
        unsigned short line_thickness;
        struct nk_vec2i begin;
        struct nk_vec2i end;
        struct nk_vec2i ctrl[2];
        struct nk_color color;
    };

    struct nk_command_rect {
        struct nk_command header;
        unsigned short rounding;
        unsigned short line_thickness;
        short x, y;
        unsigned short w, h;
        struct nk_color color;
    };

    struct nk_command_rect_filled {
        struct nk_command header;
        unsigned short rounding;
        short x, y;
        unsigned short w, h;
        struct nk_color color;
    };

    struct nk_command_rect_multi_color {
        struct nk_command header;
        short x, y;
        unsigned short w, h;
        struct nk_color left;
        struct nk_color top;
        struct nk_color bottom;
        struct nk_color right;
    };

    struct nk_command_triangle {
        struct nk_command header;
        unsigned short line_thickness;
        struct nk_vec2i a;
        struct nk_vec2i b;
        struct nk_vec2i c;
        struct nk_color color;
    };

    struct nk_command_triangle_filled {
        struct nk_command header;
        struct nk_vec2i a;
        struct nk_vec2i b;
        struct nk_vec2i c;
        struct nk_color color;
    };

    struct nk_command_circle {
        struct nk_command header;
        short x, y;
        unsigned short line_thickness;
        unsigned short w, h;
        struct nk_color color;
    };

    struct nk_command_circle_filled {
        struct nk_command header;
        short x, y;
        unsigned short w, h;
        struct nk_color color;
    };

    struct nk_command_arc {
        struct nk_command header;
        short cx, cy;
        unsigned short r;
        unsigned short line_thickness;
        float a[2];
        struct nk_color color;
    };

    struct nk_command_arc_filled {
        struct nk_command header;
        short cx, cy;
        unsigned short r;
        float a[2];
        struct nk_color color;
    };

    struct nk_command_polygon {
        struct nk_command header;
        struct nk_color color;
        unsigned short line_thickness;
        unsigned short point_count;
        struct nk_vec2i points[1];
    };

    struct nk_command_polygon_filled {
        struct nk_command header;
        struct nk_color color;
        unsigned short point_count;
        struct nk_vec2i points[1];
    };

    struct nk_command_polyline {
        struct nk_command header;
        struct nk_color color;
        unsigned short line_thickness;
        unsigned short point_count;
        struct nk_vec2i points[1];
    };

    struct nk_command_image {
        struct nk_command header;
        short x, y;
        unsigned short w, h;
        struct nk_image img;
        struct nk_color col;
    };

    typedef void(*nk_command_custom_callback)(void *canvas, short x, short y,
        unsigned short w, unsigned short h, nk_handle callback_data);
    struct nk_command_custom {
        struct nk_command header;
        short x, y;
        unsigned short w, h;
        nk_handle callback_data;
        nk_command_custom_callback callback;
    };

    struct nk_command_text {
        struct nk_command header;
        const struct nk_user_font *font;
        struct nk_color background;
        struct nk_color foreground;
        short x, y;
        unsigned short w, h;
        float height;
        int length;
        char string[1];
    };

    enum nk_command_clipping {
        NK_CLIPPING_OFF = nk_false,
        NK_CLIPPING_ON = nk_true
    };

    struct nk_command_buffer {
        struct nk_buffer *base;
        struct nk_rect clip;
        int use_clipping;
        nk_handle userdata;
        nk_size begin, end, last;
    };

    /* shape outlines */
    NK_API void nk_stroke_line(struct nk_command_buffer *b, float x0, float y0, float x1, float y1, float line_thickness, struct nk_color);
    NK_API void nk_stroke_curve(struct nk_command_buffer*, float, float, float, float, float, float, float, float, float line_thickness, struct nk_color);
    NK_API void nk_stroke_rect(struct nk_command_buffer*, struct nk_rect, float rounding, float line_thickness, struct nk_color);
    NK_API void nk_stroke_circle(struct nk_command_buffer*, struct nk_rect, float line_thickness, struct nk_color);
    NK_API void nk_stroke_arc(struct nk_command_buffer*, float cx, float cy, float radius, float a_min, float a_max, float line_thickness, struct nk_color);
    NK_API void nk_stroke_triangle(struct nk_command_buffer*, float, float, float, float, float, float, float line_thichness, struct nk_color);
    NK_API void nk_stroke_polyline(struct nk_command_buffer*, float *points, int point_count, float line_thickness, struct nk_color col);
    NK_API void nk_stroke_polygon(struct nk_command_buffer*, float*, int point_count, float line_thickness, struct nk_color);

    /* filled shades */
    NK_API void nk_fill_rect(struct nk_command_buffer*, struct nk_rect, float rounding, struct nk_color);
    NK_API void nk_fill_rect_multi_color(struct nk_command_buffer*, struct nk_rect, struct nk_color left, struct nk_color top, struct nk_color right, struct nk_color bottom);
    NK_API void nk_fill_circle(struct nk_command_buffer*, struct nk_rect, struct nk_color);
    NK_API void nk_fill_arc(struct nk_command_buffer*, float cx, float cy, float radius, float a_min, float a_max, struct nk_color);
    NK_API void nk_fill_triangle(struct nk_command_buffer*, float x0, float y0, float x1, float y1, float x2, float y2, struct nk_color);
    NK_API void nk_fill_polygon(struct nk_command_buffer*, float*, int point_count, struct nk_color);

    /* misc */
    NK_API void nk_draw_image(struct nk_command_buffer*, struct nk_rect, const struct nk_image*, struct nk_color);
    NK_API void nk_draw_text(struct nk_command_buffer*, struct nk_rect, const char *text, int len, const struct nk_user_font*, struct nk_color, struct nk_color);
    NK_API void nk_push_scissor(struct nk_command_buffer*, struct nk_rect);
    NK_API void nk_push_custom(struct nk_command_buffer*, struct nk_rect, nk_command_custom_callback, nk_handle usr);

    /* ===============================================================
    *
    *                          INPUT
    *
    * ===============================================================*/
    struct nk_mouse_button {
        int down;
        unsigned int clicked;
        struct nk_vec2 clicked_pos;
    };
    struct nk_mouse {
        struct nk_mouse_button buttons[NK_BUTTON_MAX];
        struct nk_vec2 pos;
        struct nk_vec2 prev;
        struct nk_vec2 delta;
        struct nk_vec2 scroll_delta;
        unsigned char grab;
        unsigned char grabbed;
        unsigned char ungrab;
    };

    struct nk_key {
        int down;
        unsigned int clicked;
    };
    struct nk_keyboard {
        struct nk_key keys[NK_KEY_MAX];
        char text[NK_INPUT_MAX];
        int text_len;
    };

    struct nk_input {
        struct nk_keyboard keyboard;
        struct nk_mouse mouse;
    };

    NK_API int nk_input_has_mouse_click(const struct nk_input*, enum nk_buttons);
    NK_API int nk_input_has_mouse_click_in_rect(const struct nk_input*, enum nk_buttons, struct nk_rect);
    NK_API int nk_input_has_mouse_click_down_in_rect(const struct nk_input*, enum nk_buttons, struct nk_rect, int down);
    NK_API int nk_input_is_mouse_click_in_rect(const struct nk_input*, enum nk_buttons, struct nk_rect);
    NK_API int nk_input_is_mouse_click_down_in_rect(const struct nk_input *i, enum nk_buttons id, struct nk_rect b, int down);
    NK_API int nk_input_any_mouse_click_in_rect(const struct nk_input*, struct nk_rect);
    NK_API int nk_input_is_mouse_prev_hovering_rect(const struct nk_input*, struct nk_rect);
    NK_API int nk_input_is_mouse_hovering_rect(const struct nk_input*, struct nk_rect);
    NK_API int nk_input_mouse_clicked(const struct nk_input*, enum nk_buttons, struct nk_rect);
    NK_API int nk_input_is_mouse_down(const struct nk_input*, enum nk_buttons);
    NK_API int nk_input_is_mouse_pressed(const struct nk_input*, enum nk_buttons);
    NK_API int nk_input_is_mouse_released(const struct nk_input*, enum nk_buttons);
    NK_API int nk_input_is_key_pressed(const struct nk_input*, enum nk_keys);
    NK_API int nk_input_is_key_released(const struct nk_input*, enum nk_keys);
    NK_API int nk_input_is_key_down(const struct nk_input*, enum nk_keys);

    /* ===============================================================
    *
    *                          DRAW LIST
    *
    * ===============================================================*/
#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
    /*  The optional vertex buffer draw list provides a 2D drawing context
    with antialiasing functionality which takes basic filled or outlined shapes
    or a path and outputs vertexes, elements and draw commands.
    The actual draw list API is not required to be used directly while using this
    library since converting the default library draw command output is done by
    just calling `nk_convert` but I decided to still make this library accessible
    since it can be useful.

    The draw list is based on a path buffering and polygon and polyline
    rendering API which allows a lot of ways to draw 2D content to screen.
    In fact it is probably more powerful than needed but allows even more crazy
    things than this library provides by default.
    */
    typedef nk_ushort nk_draw_index;
    enum nk_draw_list_stroke {
        NK_STROKE_OPEN = nk_false,
        /* build up path has no connection back to the beginning */
        NK_STROKE_CLOSED = nk_true
        /* build up path has a connection back to the beginning */
    };

    enum nk_draw_vertex_layout_attribute {
        NK_VERTEX_POSITION,
        NK_VERTEX_COLOR,
        NK_VERTEX_TEXCOORD,
        NK_VERTEX_ATTRIBUTE_COUNT
    };

    enum nk_draw_vertex_layout_format {
        NK_FORMAT_SCHAR,
        NK_FORMAT_SSHORT,
        NK_FORMAT_SINT,
        NK_FORMAT_UCHAR,
        NK_FORMAT_USHORT,
        NK_FORMAT_UINT,
        NK_FORMAT_FLOAT,
        NK_FORMAT_DOUBLE,

        NK_FORMAT_COLOR_BEGIN,
        NK_FORMAT_R8G8B8 = NK_FORMAT_COLOR_BEGIN,
        NK_FORMAT_R16G15B16,
        NK_FORMAT_R32G32B32,

        NK_FORMAT_R8G8B8A8,
        NK_FORMAT_R16G15B16A16,
        NK_FORMAT_R32G32B32A32,
        NK_FORMAT_R32G32B32A32_FLOAT,
        NK_FORMAT_R32G32B32A32_DOUBLE,

        NK_FORMAT_RGB32,
        NK_FORMAT_RGBA32,
        NK_FORMAT_COLOR_END = NK_FORMAT_RGBA32,
        NK_FORMAT_COUNT
    };

#define NK_VERTEX_LAYOUT_END NK_VERTEX_ATTRIBUTE_COUNT,NK_FORMAT_COUNT,0
    struct nk_draw_vertex_layout_element {
        enum nk_draw_vertex_layout_attribute attribute;
        enum nk_draw_vertex_layout_format format;
        nk_size offset;
    };

    struct nk_draw_command {
        unsigned int elem_count;
        /* number of elements in the current draw batch */
        struct nk_rect clip_rect;
        /* current screen clipping rectangle */
        nk_handle texture;
        /* current texture to set */
#ifdef NK_INCLUDE_COMMAND_USERDATA
        nk_handle userdata;
#endif
    };

    struct nk_draw_list {
        struct nk_rect clip_rect;
        struct nk_vec2 circle_vtx[12];
        struct nk_convert_config config;

        struct nk_buffer *buffer;
        struct nk_buffer *vertices;
        struct nk_buffer *elements;

        unsigned int element_count;
        unsigned int vertex_count;
        unsigned int cmd_count;
        nk_size cmd_offset;

        unsigned int path_count;
        unsigned int path_offset;

#ifdef NK_INCLUDE_COMMAND_USERDATA
        nk_handle userdata;
#endif
    };

    /* draw list */
    NK_API void nk_draw_list_init(struct nk_draw_list*);
    NK_API void nk_draw_list_setup(struct nk_draw_list*, const struct nk_convert_config*, struct nk_buffer *cmds, struct nk_buffer *vertices, struct nk_buffer *elements);
    NK_API void nk_draw_list_clear(struct nk_draw_list*);

    /* drawing */
#define nk_draw_list_foreach(cmd, can, b) for((cmd)=nk__draw_list_begin(can, b); (cmd)!=0; (cmd)=nk__draw_list_next(cmd, b, can))
    NK_API const struct nk_draw_command* nk__draw_list_begin(const struct nk_draw_list*, const struct nk_buffer*);
    NK_API const struct nk_draw_command* nk__draw_list_next(const struct nk_draw_command*, const struct nk_buffer*, const struct nk_draw_list*);
    NK_API const struct nk_draw_command* nk__draw_list_end(const struct nk_draw_list*, const struct nk_buffer*);
    NK_API void nk_draw_list_clear(struct nk_draw_list *list);

    /* path */
    NK_API void nk_draw_list_path_clear(struct nk_draw_list*);
    NK_API void nk_draw_list_path_line_to(struct nk_draw_list*, struct nk_vec2 pos);
    NK_API void nk_draw_list_path_arc_to_fast(struct nk_draw_list*, struct nk_vec2 center, float radius, int a_min, int a_max);
    NK_API void nk_draw_list_path_arc_to(struct nk_draw_list*, struct nk_vec2 center, float radius, float a_min, float a_max, unsigned int segments);
    NK_API void nk_draw_list_path_rect_to(struct nk_draw_list*, struct nk_vec2 a, struct nk_vec2 b, float rounding);
    NK_API void nk_draw_list_path_curve_to(struct nk_draw_list*, struct nk_vec2 p2, struct nk_vec2 p3, struct nk_vec2 p4, unsigned int num_segments);
    NK_API void nk_draw_list_path_fill(struct nk_draw_list*, struct nk_color);
    NK_API void nk_draw_list_path_stroke(struct nk_draw_list*, struct nk_color, enum nk_draw_list_stroke closed, float thickness);

    /* stroke */
    NK_API void nk_draw_list_stroke_line(struct nk_draw_list*, struct nk_vec2 a, struct nk_vec2 b, struct nk_color, float thickness);
    NK_API void nk_draw_list_stroke_rect(struct nk_draw_list*, struct nk_rect rect, struct nk_color, float rounding, float thickness);
    NK_API void nk_draw_list_stroke_triangle(struct nk_draw_list*, struct nk_vec2 a, struct nk_vec2 b, struct nk_vec2 c, struct nk_color, float thickness);
    NK_API void nk_draw_list_stroke_circle(struct nk_draw_list*, struct nk_vec2 center, float radius, struct nk_color, unsigned int segs, float thickness);
    NK_API void nk_draw_list_stroke_curve(struct nk_draw_list*, struct nk_vec2 p0, struct nk_vec2 cp0, struct nk_vec2 cp1, struct nk_vec2 p1, struct nk_color, unsigned int segments, float thickness);
    NK_API void nk_draw_list_stroke_poly_line(struct nk_draw_list*, const struct nk_vec2 *pnts, const unsigned int cnt, struct nk_color, enum nk_draw_list_stroke, float thickness, enum nk_anti_aliasing);

    /* fill */
    NK_API void nk_draw_list_fill_rect(struct nk_draw_list*, struct nk_rect rect, struct nk_color, float rounding);
    NK_API void nk_draw_list_fill_rect_multi_color(struct nk_draw_list*, struct nk_rect rect, struct nk_color left, struct nk_color top, struct nk_color right, struct nk_color bottom);
    NK_API void nk_draw_list_fill_triangle(struct nk_draw_list*, struct nk_vec2 a, struct nk_vec2 b, struct nk_vec2 c, struct nk_color);
    NK_API void nk_draw_list_fill_circle(struct nk_draw_list*, struct nk_vec2 center, float radius, struct nk_color col, unsigned int segs);
    NK_API void nk_draw_list_fill_poly_convex(struct nk_draw_list*, const struct nk_vec2 *points, const unsigned int count, struct nk_color, enum nk_anti_aliasing);

    /* misc */
    NK_API void nk_draw_list_add_image(struct nk_draw_list*, struct nk_image texture, struct nk_rect rect, struct nk_color);
    NK_API void nk_draw_list_add_text(struct nk_draw_list*, const struct nk_user_font*, struct nk_rect, const char *text, int len, float font_height, struct nk_color);
#ifdef NK_INCLUDE_COMMAND_USERDATA
    NK_API void nk_draw_list_push_userdata(struct nk_draw_list*, nk_handle userdata);
#endif

#endif

    /* ===============================================================
    *
    *                          GUI
    *
    * ===============================================================*/
    enum nk_style_item_type {
        NK_STYLE_ITEM_COLOR,
        NK_STYLE_ITEM_IMAGE
    };

    union nk_style_item_data {
        struct nk_image image;
        struct nk_color color;
    };

    struct nk_style_item {
        enum nk_style_item_type type;
        union nk_style_item_data data;
    };

    struct nk_style_text {
        struct nk_color color;
        struct nk_vec2 padding;
    };

    struct nk_style_button {
        /* background */
        struct nk_style_item normal;
        struct nk_style_item hover;
        struct nk_style_item active;
        struct nk_color border_color;

        /* text */
        struct nk_color text_background;
        struct nk_color text_normal;
        struct nk_color text_hover;
        struct nk_color text_active;
        nk_flags text_alignment;

        /* properties */
        float border;
        float rounding;
        struct nk_vec2 padding;
        struct nk_vec2 image_padding;
        struct nk_vec2 touch_padding;

        /* optional user callbacks */
        nk_handle userdata;
        void(*draw_begin)(struct nk_command_buffer*, nk_handle userdata);
        void(*draw_end)(struct nk_command_buffer*, nk_handle userdata);
    };

    struct nk_style_toggle {
        /* background */
        struct nk_style_item normal;
        struct nk_style_item hover;
        struct nk_style_item active;
        struct nk_color border_color;

        /* cursor */
        struct nk_style_item cursor_normal;
        struct nk_style_item cursor_hover;

        /* text */
        struct nk_color text_normal;
        struct nk_color text_hover;
        struct nk_color text_active;
        struct nk_color text_background;
        nk_flags text_alignment;

        /* properties */
        struct nk_vec2 padding;
        struct nk_vec2 touch_padding;
        float spacing;
        float border;

        /* optional user callbacks */
        nk_handle userdata;
        void(*draw_begin)(struct nk_command_buffer*, nk_handle);
        void(*draw_end)(struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_selectable {
        /* background (inactive) */
        struct nk_style_item normal;
        struct nk_style_item hover;
        struct nk_style_item pressed;

        /* background (active) */
        struct nk_style_item normal_active;
        struct nk_style_item hover_active;
        struct nk_style_item pressed_active;

        /* text color (inactive) */
        struct nk_color text_normal;
        struct nk_color text_hover;
        struct nk_color text_pressed;

        /* text color (active) */
        struct nk_color text_normal_active;
        struct nk_color text_hover_active;
        struct nk_color text_pressed_active;
        struct nk_color text_background;
        nk_flags text_alignment;

        /* properties */
        float rounding;
        struct nk_vec2 padding;
        struct nk_vec2 touch_padding;
        struct nk_vec2 image_padding;

        /* optional user callbacks */
        nk_handle userdata;
        void(*draw_begin)(struct nk_command_buffer*, nk_handle);
        void(*draw_end)(struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_slider {
        /* background */
        struct nk_style_item normal;
        struct nk_style_item hover;
        struct nk_style_item active;
        struct nk_color border_color;

        /* background bar */
        struct nk_color bar_normal;
        struct nk_color bar_hover;
        struct nk_color bar_active;
        struct nk_color bar_filled;

        /* cursor */
        struct nk_style_item cursor_normal;
        struct nk_style_item cursor_hover;
        struct nk_style_item cursor_active;

        /* properties */
        float border;
        float rounding;
        float bar_height;
        struct nk_vec2 padding;
        struct nk_vec2 spacing;
        struct nk_vec2 cursor_size;

        /* optional buttons */
        int show_buttons;
        struct nk_style_button inc_button;
        struct nk_style_button dec_button;
        enum nk_symbol_type inc_symbol;
        enum nk_symbol_type dec_symbol;

        /* optional user callbacks */
        nk_handle userdata;
        void(*draw_begin)(struct nk_command_buffer*, nk_handle);
        void(*draw_end)(struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_progress {
        /* background */
        struct nk_style_item normal;
        struct nk_style_item hover;
        struct nk_style_item active;
        struct nk_color border_color;

        /* cursor */
        struct nk_style_item cursor_normal;
        struct nk_style_item cursor_hover;
        struct nk_style_item cursor_active;
        struct nk_color cursor_border_color;

        /* properties */
        float rounding;
        float border;
        float cursor_border;
        float cursor_rounding;
        struct nk_vec2 padding;

        /* optional user callbacks */
        nk_handle userdata;
        void(*draw_begin)(struct nk_command_buffer*, nk_handle);
        void(*draw_end)(struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_scrollbar {
        /* background */
        struct nk_style_item normal;
        struct nk_style_item hover;
        struct nk_style_item active;
        struct nk_color border_color;

        /* cursor */
        struct nk_style_item cursor_normal;
        struct nk_style_item cursor_hover;
        struct nk_style_item cursor_active;
        struct nk_color cursor_border_color;

        /* properties */
        float border;
        float rounding;
        float border_cursor;
        float rounding_cursor;
        struct nk_vec2 padding;

        /* optional buttons */
        int show_buttons;
        struct nk_style_button inc_button;
        struct nk_style_button dec_button;
        enum nk_symbol_type inc_symbol;
        enum nk_symbol_type dec_symbol;

        /* optional user callbacks */
        nk_handle userdata;
        void(*draw_begin)(struct nk_command_buffer*, nk_handle);
        void(*draw_end)(struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_edit {
        /* background */
        struct nk_style_item normal;
        struct nk_style_item hover;
        struct nk_style_item active;
        struct nk_color border_color;
        struct nk_style_scrollbar scrollbar;

        /* cursor  */
        struct nk_color cursor_normal;
        struct nk_color cursor_hover;
        struct nk_color cursor_text_normal;
        struct nk_color cursor_text_hover;

        /* text (unselected) */
        struct nk_color text_normal;
        struct nk_color text_hover;
        struct nk_color text_active;

        /* text (selected) */
        struct nk_color selected_normal;
        struct nk_color selected_hover;
        struct nk_color selected_text_normal;
        struct nk_color selected_text_hover;

        /* properties */
        float border;
        float rounding;
        float cursor_size;
        struct nk_vec2 scrollbar_size;
        struct nk_vec2 padding;
        float row_padding;
    };

    struct nk_style_property {
        /* background */
        struct nk_style_item normal;
        struct nk_style_item hover;
        struct nk_style_item active;
        struct nk_color border_color;

        /* text */
        struct nk_color label_normal;
        struct nk_color label_hover;
        struct nk_color label_active;

        /* symbols */
        enum nk_symbol_type sym_left;
        enum nk_symbol_type sym_right;

        /* properties */
        float border;
        float rounding;
        struct nk_vec2 padding;

        struct nk_style_edit edit;
        struct nk_style_button inc_button;
        struct nk_style_button dec_button;

        /* optional user callbacks */
        nk_handle userdata;
        void(*draw_begin)(struct nk_command_buffer*, nk_handle);
        void(*draw_end)(struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_chart {
        /* colors */
        struct nk_style_item background;
        struct nk_color border_color;
        struct nk_color selected_color;
        struct nk_color color;

        /* properties */
        float border;
        float rounding;
        struct nk_vec2 padding;
    };

    struct nk_style_combo {
        /* background */
        struct nk_style_item normal;
        struct nk_style_item hover;
        struct nk_style_item active;
        struct nk_color border_color;

        /* label */
        struct nk_color label_normal;
        struct nk_color label_hover;
        struct nk_color label_active;

        /* symbol */
        struct nk_color symbol_normal;
        struct nk_color symbol_hover;
        struct nk_color symbol_active;

        /* button */
        struct nk_style_button button;
        enum nk_symbol_type sym_normal;
        enum nk_symbol_type sym_hover;
        enum nk_symbol_type sym_active;

        /* properties */
        float border;
        float rounding;
        struct nk_vec2 content_padding;
        struct nk_vec2 button_padding;
        struct nk_vec2 spacing;
    };

    struct nk_style_tab {
        /* background */
        struct nk_style_item background;
        struct nk_color border_color;
        struct nk_color text;

        /* button */
        struct nk_style_button tab_maximize_button;
        struct nk_style_button tab_minimize_button;
        struct nk_style_button node_maximize_button;
        struct nk_style_button node_minimize_button;
        enum nk_symbol_type sym_minimize;
        enum nk_symbol_type sym_maximize;

        /* properties */
        float border;
        float rounding;
        float indent;
        struct nk_vec2 padding;
        struct nk_vec2 spacing;
    };

    enum nk_style_header_align {
        NK_HEADER_LEFT,
        NK_HEADER_RIGHT
    };
    struct nk_style_window_header {
        /* background */
        struct nk_style_item normal;
        struct nk_style_item hover;
        struct nk_style_item active;

        /* button */
        struct nk_style_button close_button;
        struct nk_style_button minimize_button;
        enum nk_symbol_type close_symbol;
        enum nk_symbol_type minimize_symbol;
        enum nk_symbol_type maximize_symbol;

        /* title */
        struct nk_color label_normal;
        struct nk_color label_hover;
        struct nk_color label_active;

        /* properties */
        enum nk_style_header_align align;
        struct nk_vec2 padding;
        struct nk_vec2 label_padding;
        struct nk_vec2 spacing;
    };

    struct nk_style_window {
        struct nk_style_window_header header;
        struct nk_style_item fixed_background;
        struct nk_color background;

        struct nk_color border_color;
        struct nk_color popup_border_color;
        struct nk_color combo_border_color;
        struct nk_color contextual_border_color;
        struct nk_color menu_border_color;
        struct nk_color group_border_color;
        struct nk_color tooltip_border_color;
        struct nk_style_item scaler;

        float border;
        float combo_border;
        float contextual_border;
        float menu_border;
        float group_border;
        float tooltip_border;
        float popup_border;

        float rounding;
        struct nk_vec2 spacing;
        struct nk_vec2 scrollbar_size;
        struct nk_vec2 min_size;

        struct nk_vec2 padding;
        struct nk_vec2 group_padding;
        struct nk_vec2 popup_padding;
        struct nk_vec2 combo_padding;
        struct nk_vec2 contextual_padding;
        struct nk_vec2 menu_padding;
        struct nk_vec2 tooltip_padding;
    };

    struct nk_style {
        const struct nk_user_font *font;
        const struct nk_cursor *cursors[NK_CURSOR_COUNT];
        const struct nk_cursor *cursor_active;
        struct nk_cursor *cursor_last;
        int cursor_visible;

        struct nk_style_text text;
        struct nk_style_button button;
        struct nk_style_button contextual_button;
        struct nk_style_button menu_button;
        struct nk_style_toggle option;
        struct nk_style_toggle checkbox;
        struct nk_style_selectable selectable;
        struct nk_style_slider slider;
        struct nk_style_progress progress;
        struct nk_style_property property;
        struct nk_style_edit edit;
        struct nk_style_chart chart;
        struct nk_style_scrollbar scrollh;
        struct nk_style_scrollbar scrollv;
        struct nk_style_tab tab;
        struct nk_style_combo combo;
        struct nk_style_window window;
    };

    NK_API struct nk_style_item nk_style_item_image(struct nk_image img);
    NK_API struct nk_style_item nk_style_item_color(struct nk_color);
    NK_API struct nk_style_item nk_style_item_hide(void);

    /*==============================================================
    *                          PANEL
    * =============================================================*/
#ifndef NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS
#define NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS 32
#endif
#ifndef NK_CHART_MAX_SLOT
#define NK_CHART_MAX_SLOT 4
#endif

    enum nk_panel_type {
        NK_PANEL_WINDOW = NK_FLAG(0),
        NK_PANEL_GROUP = NK_FLAG(1),
        NK_PANEL_POPUP = NK_FLAG(2),
        NK_PANEL_CONTEXTUAL = NK_FLAG(4),
        NK_PANEL_COMBO = NK_FLAG(5),
        NK_PANEL_MENU = NK_FLAG(6),
        NK_PANEL_TOOLTIP = NK_FLAG(7)
    };
    enum nk_panel_set {
        NK_PANEL_SET_NONBLOCK = NK_PANEL_CONTEXTUAL | NK_PANEL_COMBO | NK_PANEL_MENU | NK_PANEL_TOOLTIP,
        NK_PANEL_SET_POPUP = NK_PANEL_SET_NONBLOCK | NK_PANEL_POPUP,
        NK_PANEL_SET_SUB = NK_PANEL_SET_POPUP | NK_PANEL_GROUP
    };

    struct nk_chart_slot {
        enum nk_chart_type type;
        struct nk_color color;
        struct nk_color highlight;
        float min, max, range;
        int count;
        struct nk_vec2 last;
        int index;
    };

    struct nk_chart {
        int slot;
        float x, y, w, h;
        struct nk_chart_slot slots_[NK_CHART_MAX_SLOT];
    };

    enum nk_panel_row_layout_type {
        NK_LAYOUT_DYNAMIC_FIXED = 0,
        NK_LAYOUT_DYNAMIC_ROW,
        NK_LAYOUT_DYNAMIC_FREE,
        NK_LAYOUT_DYNAMIC,
        NK_LAYOUT_STATIC_FIXED,
        NK_LAYOUT_STATIC_ROW,
        NK_LAYOUT_STATIC_FREE,
        NK_LAYOUT_STATIC,
        NK_LAYOUT_TEMPLATE,
        NK_LAYOUT_COUNT
    };

    struct nk_row_template {
        float width;
        nk_layout_valign valign;
        nk_layout_halign halign;
    };

    struct nk_row_layout {
        enum nk_panel_row_layout_type type;
        int index;
        float height;
        int columns;
        const float *ratio;
        float item_width;
        float item_height;
        float item_offset;
        float filled;
        struct nk_rect item;
        int tree_depth;
        nk_row_template templates[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
    };

    struct nk_popup_buffer {
        nk_size begin;
        nk_size parent;
        nk_size last;
        nk_size end;
        int active;
    };

    struct nk_menu_state {
        float x, y, w, h;
        struct nk_scroll offset;
    };

    struct nk_panel {
        enum nk_panel_type type;
        nk_flags flags;
        struct nk_rect bounds;
        nk_uint *offset_x;
        nk_uint *offset_y;
        float at_x, at_y, max_x;
        float footer_height;
        float header_height;
        float border;
        unsigned int has_scrolling;
        struct nk_rect clip;
        struct nk_menu_state menu;
        struct nk_row_layout row;
        struct nk_chart chart;
        struct nk_command_buffer *buffer;
        struct nk_panel *parent;
    };

    /*==============================================================
    *                          WINDOW
    * =============================================================*/
#ifndef NK_WINDOW_MAX_NAME
#define NK_WINDOW_MAX_NAME 64
#endif

    struct nk_table;
    enum nk_window_flags {
        NK_WINDOW_PRIVATE = NK_FLAG(11),
        NK_WINDOW_DYNAMIC = NK_WINDOW_PRIVATE,
        /* special window type growing up in height while being filled to a certain maximum height */
        NK_WINDOW_ROM = NK_FLAG(12),
        /* sets window widgets into a read only mode and does not allow input changes */
        NK_WINDOW_NOT_INTERACTIVE = NK_WINDOW_ROM | NK_WINDOW_NO_INPUT,
        /* prevents all interaction caused by input to either window or widgets inside */
        NK_WINDOW_HIDDEN = NK_FLAG(13),
        /* Hides window and stops any window interaction and drawing */
        NK_WINDOW_CLOSED = NK_FLAG(14),
        /* Directly closes and frees the window at the end of the frame */
        NK_WINDOW_MINIMIZED = NK_FLAG(15),
        /* marks the window as minimized */
        NK_WINDOW_REMOVE_ROM = NK_FLAG(16)
        /* Removes read only mode at the end of the window */
    };

    struct nk_popup_state {
        struct nk_window *win;
        enum nk_panel_type type;
        struct nk_popup_buffer buf;
        nk_hash name;
        int active;
        unsigned combo_count;
        unsigned con_count, con_old;
        unsigned active_con;
        struct nk_rect header;
    };

    struct nk_edit_state {
        nk_hash name;
        unsigned int seq;
        unsigned int old;
        int active, prev;
        int cursor;
        int sel_start;
        int sel_end;
        struct nk_scroll scrollbar;
        unsigned char mode;
        unsigned char single_line;
    };

    struct nk_property_state {
        int active, prev;
        char buffer[NK_MAX_NUMBER_BUFFER];
        int length;
        int cursor;
        int select_start;
        int select_end;
        nk_hash name;
        unsigned int seq;
        unsigned int old;
        int state;
    };

    struct nk_window {
        unsigned int seq;
        nk_hash name;
        char name_string[NK_WINDOW_MAX_NAME];
        nk_flags flags;

        struct nk_rect bounds;
        struct nk_scroll scrollbar;
        struct nk_command_buffer buffer;
        struct nk_panel *layout;
        float scrollbar_hiding_timer;

        /* persistent widget state */
        struct nk_property_state property;
        struct nk_popup_state popup;
        struct nk_edit_state edit;
        unsigned int scrolled;

        struct nk_table *tables;
        unsigned short table_count;
        unsigned short table_size;

        /* window list hooks */
        struct nk_window *next;
        struct nk_window *prev;
        struct nk_window *parent;
    };

    /*==============================================================
    *                          STACK
    * =============================================================*/
    /* The style modifier stack can be used to temporarily change a
    * property inside `nk_style`. For example if you want a special
    * red button you can temporarily push the old button color onto a stack
    * draw the button with a red color and then you just pop the old color
    * back from the stack:
    *
    *      nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(255,0,0)));
    *      nk_style_push_style_item(ctx, &ctx->style.button.hover, nk_style_item_color(nk_rgb(255,0,0)));
    *      nk_style_push_style_item(ctx, &ctx->style.button.active, nk_style_item_color(nk_rgb(255,0,0)));
    *      nk_style_push_vec2(ctx, &cx->style.button.padding, nk_vec2(2,2));
    *
    *      nk_button(...);
    *
    *      nk_style_pop_style_item(ctx);
    *      nk_style_pop_style_item(ctx);
    *      nk_style_pop_style_item(ctx);
    *      nk_style_pop_vec2(ctx);
    *
    * Nuklear has a stack for style_items, float properties, vector properties,
    * flags, colors, fonts and for button_behavior. Each has it's own fixed size stack
    * which can be changed at compile time.
    */
#ifndef NK_BUTTON_BEHAVIOR_STACK_SIZE
#define NK_BUTTON_BEHAVIOR_STACK_SIZE 8
#endif

#ifndef NK_FONT_STACK_SIZE
#define NK_FONT_STACK_SIZE 8
#endif

#ifndef NK_STYLE_ITEM_STACK_SIZE
#define NK_STYLE_ITEM_STACK_SIZE 16
#endif

#ifndef NK_FLOAT_STACK_SIZE
#define NK_FLOAT_STACK_SIZE 32
#endif

#ifndef NK_VECTOR_STACK_SIZE
#define NK_VECTOR_STACK_SIZE 16
#endif

#ifndef NK_FLAGS_STACK_SIZE
#define NK_FLAGS_STACK_SIZE 32
#endif

#ifndef NK_COLOR_STACK_SIZE
#define NK_COLOR_STACK_SIZE 32
#endif

#define NK_CONFIGURATION_STACK_TYPE(prefix, name, type)\
    struct nk_config_stack_##name##_element {\
        prefix##_##type *address;\
        prefix##_##type old_value;\
    }
#define NK_CONFIG_STACK(type,size)\
    struct nk_config_stack_##type {\
        int head;\
        struct nk_config_stack_##type##_element elements[size];\
    }

#define nk_float float
    NK_CONFIGURATION_STACK_TYPE(struct nk, style_item, style_item);
    NK_CONFIGURATION_STACK_TYPE(nk, float, float);
    NK_CONFIGURATION_STACK_TYPE(struct nk, vec2, vec2);
    NK_CONFIGURATION_STACK_TYPE(nk, flags, flags);
    NK_CONFIGURATION_STACK_TYPE(struct nk, color, color);
    NK_CONFIGURATION_STACK_TYPE(const struct nk, user_font, user_font*);
    NK_CONFIGURATION_STACK_TYPE(enum nk, button_behavior, button_behavior);

    NK_CONFIG_STACK(style_item, NK_STYLE_ITEM_STACK_SIZE);
    NK_CONFIG_STACK(float, NK_FLOAT_STACK_SIZE);
    NK_CONFIG_STACK(vec2, NK_VECTOR_STACK_SIZE);
    NK_CONFIG_STACK(flags, NK_FLAGS_STACK_SIZE);
    NK_CONFIG_STACK(color, NK_COLOR_STACK_SIZE);
    NK_CONFIG_STACK(user_font, NK_FONT_STACK_SIZE);
    NK_CONFIG_STACK(button_behavior, NK_BUTTON_BEHAVIOR_STACK_SIZE);

    struct nk_configuration_stacks {
        struct nk_config_stack_style_item style_items;
        struct nk_config_stack_float floats;
        struct nk_config_stack_vec2 vectors;
        struct nk_config_stack_flags flags;
        struct nk_config_stack_color colors;
        struct nk_config_stack_user_font fonts;
        struct nk_config_stack_button_behavior button_behaviors;
    };

    /*==============================================================
    *                          CONTEXT
    * =============================================================*/
#define NK_VALUE_PAGE_CAPACITY \
    ((NK_MAX(sizeof(struct nk_window),sizeof(struct nk_panel)) / sizeof(nk_uint)) / 2)

    struct nk_table {
        unsigned int seq;
        nk_hash keys[NK_VALUE_PAGE_CAPACITY];
        nk_uint values[NK_VALUE_PAGE_CAPACITY];
        struct nk_table *next, *prev;
    };

    union nk_page_data {
        struct nk_table tbl;
        struct nk_panel pan;
        struct nk_window win;
    };

    struct nk_page_element {
        union nk_page_data data;
        struct nk_page_element *next;
        struct nk_page_element *prev;
    };

    struct nk_page {
        unsigned int size;
        struct nk_page *next;
        struct nk_page_element win[1];
    };

    struct nk_pool {
        struct nk_allocator alloc;
        enum nk_allocation_type type;
        unsigned int page_count;
        struct nk_page *pages;
        struct nk_page_element *freelist;
        unsigned capacity;
        nk_size size;
        nk_size cap;
    };

    enum nk_hotspot_flags {
        NK_HOTSPOT_NO_REPAINT = 0,
        NK_HOTSPOT_REPAINT = 1
    };

    struct nk_hotspot {
        struct nk_rect rect;
        enum nk_style_cursor cursor_style;
        int flags;
    };

    struct nk_context {
        /* public: can be accessed freely */
        struct nk_input input;
        struct nk_style style;
        struct nk_buffer memory;
        struct nk_clipboard clip;
        nk_flags last_widget_state;
        enum nk_button_behavior button_behavior;
        struct nk_configuration_stacks stacks;
        float delta_time_seconds;
        enum nk_style_cursor cursor_style;
        nk_handle hotspotHandle;
        nk_hotspot_output hotspotFunction;

        /* private:
        should only be accessed if you
        know what you are doing */
#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
        struct nk_draw_list draw_list;
#endif
#ifdef NK_INCLUDE_COMMAND_USERDATA
        nk_handle userdata;
#endif
        /* text editor objects are quite big because of an internal
        * undo/redo stack. Therefore it does not make sense to have one for
        * each window for temporary use cases, so I only provide *one* instance
        * for all windows. This works because the content is cleared anyway */
        struct nk_text_edit text_edit;
        /* draw buffer used for overlay drawing operation like cursor */
        struct nk_command_buffer overlay;

        /* windows */
        int build;
        int use_pool;
        struct nk_pool pool;
        struct nk_window *begin;
        struct nk_window *end;
        struct nk_window *active;
        struct nk_window *current;
        struct nk_page_element *freelist;
        unsigned int count;
        unsigned int seq;
    };

    /* ==============================================================
    *                          MATH
    * =============================================================== */
#define NK_PI 3.141592654f
#define NK_UTF_INVALID 0xFFFD
#define NK_MAX_FLOAT_PRECISION 2

#define NK_UNUSED(x) ((void)(x))
#define NK_SATURATE(x) (NK_MAX(0, NK_MIN(1.0f, x)))
#define NK_LEN(a) (sizeof(a)/sizeof(a)[0])
#define NK_ABS(a) (((a) < 0) ? -(a) : (a))
#define NK_BETWEEN(x, a, b) ((a) <= (x) && (x) < (b))
#define NK_INBOX(px, py, x, y, w, h)\
    (NK_BETWEEN(px,x,x+w) && NK_BETWEEN(py,y,y+h))
#define NK_INTERSECT(x0, y0, w0, h0, x1, y1, w1, h1) \
    (!(((x1 > (x0 + w0)) || ((x1 + w1) < x0) || (y1 > (y0 + h0)) || (y1 + h1) < y0)))
#define NK_CONTAINS(x, y, w, h, bx, by, bw, bh)\
    (NK_INBOX(x,y, bx, by, bw, bh) && NK_INBOX(x+w,y+h, bx, by, bw, bh))

#define nk_vec2_sub(a, b) nk_vec2((a).x - (b).x, (a).y - (b).y)
#define nk_vec2_add(a, b) nk_vec2((a).x + (b).x, (a).y + (b).y)
#define nk_vec2_len_sqr(a) ((a).x*(a).x+(a).y*(a).y)
#define nk_vec2_muls(a, t) nk_vec2((a).x * (t), (a).y * (t))

#define nk_ptr_add(t, p, i) ((t*)((void*)((nk_byte*)(p) + (i))))
#define nk_ptr_add_const(t, p, i) ((const t*)((const void*)((const nk_byte*)(p) + (i))))
#define nk_zero_struct(s) nk_zero(&s, sizeof(s))

    /* ==============================================================
    *                          ALIGNMENT
    * =============================================================== */
    /* Pointer to Integer type conversion for pointer alignment */
#if defined(__PTRDIFF_TYPE__) /* This case should work for GCC*/
# define NK_UINT_TO_PTR(x) ((void*)(__PTRDIFF_TYPE__)(x))
# define NK_PTR_TO_UINT(x) ((nk_size)(__PTRDIFF_TYPE__)(x))
#elif !defined(__GNUC__) /* works for compilers other than LLVM */
# define NK_UINT_TO_PTR(x) ((void*)&((char*)0)[x])
# define NK_PTR_TO_UINT(x) ((nk_size)(((char*)x)-(char*)0))
#elif defined(NK_USE_FIXED_TYPES) /* used if we have <stdint.h> */
# define NK_UINT_TO_PTR(x) ((void*)(uintptr_t)(x))
# define NK_PTR_TO_UINT(x) ((uintptr_t)(x))
#else /* generates warning but works */
# define NK_UINT_TO_PTR(x) ((void*)(x))
# define NK_PTR_TO_UINT(x) ((nk_size)(x))
#endif

#define NK_ALIGN_PTR(x, mask)\
    (NK_UINT_TO_PTR((NK_PTR_TO_UINT((nk_byte*)(x) + (mask-1)) & ~(mask-1))))
#define NK_ALIGN_PTR_BACK(x, mask)\
    (NK_UINT_TO_PTR((NK_PTR_TO_UINT((nk_byte*)(x)) & ~(mask-1))))

#define NK_OFFSETOF(st,m) ((nk_ptr)&(((st*)0)->m))
#define NK_CONTAINER_OF(ptr,type,member)\
    (type*)((void*)((char*)(1 ? (ptr): &((type*)0)->member) - NK_OFFSETOF(type, member)))

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
template<typename T> struct nk_alignof;
template<typename T, int size_diff> struct nk_helper { enum { value = size_diff }; };
template<typename T> struct nk_helper<T, 0> { enum { value = nk_alignof<T>::value }; };
template<typename T> struct nk_alignof {
    struct Big { T x; char c; }; enum {
        diff = sizeof(Big) - sizeof(T), value = nk_helper<Big, diff>::value
    };
};
#define NK_ALIGNOF(t) (nk_alignof<t>::value);
#elif defined(_MSC_VER)
#define NK_ALIGNOF(t) (__alignof(t))
#else
#define NK_ALIGNOF(t) ((char*)(&((struct {char c; t _h;}*)0)->_h) - (char*)0)
#endif

void nk_panel_alloc_space(struct nk_rect *bounds, const struct nk_context *ctx);

#endif
