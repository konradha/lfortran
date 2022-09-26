#ifndef LFORTRAN_AST_TO_SRC_H
#define LFORTRAN_AST_TO_SRC_H

#include <lfortran/ast.h>

namespace LFortran {

// Converts AST to Fortran source code
std::string ast_to_src(AST::TranslationUnit_t &ast, bool color = false,
                       int indent = 4, bool indent_unit = false);

}  // namespace LFortran

#endif  // LFORTRAN_AST_TO_SRC_H
