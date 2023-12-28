/**
 * @file jl.h
 * @author Joe Wingbermuehle
 *
 * Public interface to the JL interpreter.
 *
 */

#ifndef JL_H
#define JL_H

#ifdef __cplusplus
extern "C" {
#endif

#define JL_VERSION_MAJOR   0
#define JL_VERSION_MINOR   1


struct JLValue;
struct JLContext;

/** The type of special functions.
 * @param context The JL context.
 * @param args A list of arguments to the function, including its name.
 * @param extra Extra parameter from JLDefineSpecial.
 * @return The result, which should be retained (it will be freed if
 *         not needed).
 */
typedef struct JLValue *(*JLFunction)(struct JLContext *context,
                                      struct JLValue *args,
                                      void *extra);

/** Create a context for running JL programs.
 * @return The context.
 */

struct JLContext *JLCreateContext();

/** Destroy a JL context.
 * @param context The context to be destroyed.
 */

void JLDestroyContext(struct JLContext *context);

/** Create and enter a new lexical scope. */

void JLEnterScope(struct JLContext *context);

/** Leave and destroy the current lexical scope. */

void JLLeaveScope(struct JLContext *context);

/** Increase the reference count of a value.
 * @param context The context containing the value.
 * @param value The value (can be NULL).
 */

void JLRetain(struct JLContext *context, struct JLValue *value);

/** Decrease the reference count of a value.
 * This will destroy the value if the reference count reaches zero.
 * @param context The context.
 * @param value The value to be released (can be NULL).
 */

void JLRelease(struct JLContext *context, struct JLValue *value);

/** Define a value.
 * This will add a binding to the current scope.
 * @param context The context in which to define the value.
 * @param name The name of the binding.
 * @param value The value to insert.
 */

void JLDefineValue(struct JLContext *context,
                   const char *name,
                   struct JLValue *value);

/** Define a special function.
 * A special function is simply a function that is implemented outside
 * of the JL environment.
 * This will add the special function to the current scope.
 * @param context The context in which to define the function.
 * @param name The name of the function.
 * @param func The function code.
 * @param extra Extra parameter to pass to func.
 */

void JLDefineSpecial(struct JLContext *context,
                     const char *name,
                     JLFunction func,
                     void *extra);

/** Define a number.
 * This will add a number to the current scope.
 * @param context The context in which to define the number.
 * @param name The name of the binding (NULL for no name).
 * @param value The value to define.
 * @return The value.  This value must be released if not used.
 */

struct JLValue *JLDefineNumber(struct JLContext *context,
                               const char *name,
                               double value);

/** Parse an expression.
 * Note that only a single expression is parsed.
 * @param context The context.
 * @param line The expression to be parsed.
 * @return The expression.  This value must be released if not used.
 */

struct JLValue *JLParse(struct JLContext *context, const char **line);

/** Evaluate an expression.
 * @param context The context.
 * @param value The expression to evaluate.
 * @return The result.  This value must be released if not used.
 */

struct JLValue *JLEvaluate(struct JLContext *context, struct JLValue *value);

/** Determine if a value is a number.
 * @param value The value to check (NULL is allowed).
 * @return 1 if a number, 0 otherwise.
 */

char JLIsNumber(struct JLValue *value);

/** Get the number representation of a value.
 * @param value The value (must be a non-NULL number value).
 * @return The numeric value.
 */

double JLGetNumber(struct JLValue *value);

/** Determine if a value is a string.
 * @param value The value to check (NULL is allowed).
 * @return 1 if a string, 0 otherwise.
 */

char JLIsString(struct JLValue *value);

/** Get the string representation of a value.
 * @param value The value (must be a non-NULL string value).
 * @return A NULL-terminated string.
 */

const char *JLGetString(struct JLValue *value);

/** Determine if a value is a list.
 * @param value The value to check.
 * @return 1 if a list, 0 otherwise.
 */

char JLIsList(struct JLValue *value);

/** Get the first item of a list.
 * @param value The list (must be a non-NULL list value).
 * @return The first item in the list (possibly NULL).
 */

struct JLValue *JLGetHead(struct JLValue *value);

/** Get the next item in a list.
 * @param value The previous value in the list (must be non-NULL).
 * @return The next item in the list (possibly NULL).
 */

struct JLValue *JLGetNext(struct JLValue *value);

/** Display a value.
 * @param context The context.
 * @param value The value to display.
 */

void JLPrint(const struct JLContext *context, const struct JLValue *value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JL_H */
