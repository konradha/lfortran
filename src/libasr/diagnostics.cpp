#include <iomanip>
#include <sstream>

#include <libasr/diagnostics.h>
#include <libasr/assert.h>
#include <libasr/exception.h>
#include <libasr/utils.h>

namespace LFortran::diag {

const static std::string redon  = "\033[0;31m";
const static std::string redoff = "\033[0;00m";

std::string highlight_line(const std::string &line,
        const size_t first_column,
        const size_t last_column,
        bool use_colors)
{
    if (first_column == 0 || last_column == 0) return "";
    if (last_column > line.size()+1) {
        throw LCompilersException("The `last_column` in highlight_line is longer than the source line");
    }
    LFORTRAN_ASSERT(first_column >= 1)
    LFORTRAN_ASSERT(first_column <= last_column)
    LFORTRAN_ASSERT(last_column <= line.size()+1)
    std::stringstream out;
    if (line.size() > 0) {
        out << line.substr(0, first_column-1);
        if(use_colors) out << redon;
        if (last_column <= line.size()) {
            out << line.substr(first_column-1,
                    last_column-first_column+1);
        } else {
            // `last_column` points to the \n character
            out << line.substr(first_column-1,
                    last_column-first_column+1-1);
        }
        if(use_colors) out << redoff;
        if (last_column < line.size()) out << line.substr(last_column);
    }
    out << std::endl;
    if (first_column > 0) {
        for (size_t i=0; i < first_column-1; i++) {
            out << " ";
        }
    }
    if(use_colors) out << redon << "^";
    else out << "^";
    for (size_t i=first_column; i < last_column; i++) {
        out << "~";
    }
    if(use_colors) out << redoff;
    out << std::endl;
    return out.str();
}

bool Diagnostics::has_error() const {
    for (auto &d : this->diagnostics) {
        if (d.level == Level::Error) return true;
    }
    return false;
}

std::string Diagnostics::render(const std::string &input,
        const LocationManager &lm, const CompilerOptions &compiler_options) {
    std::string out;
    for (auto &d : this->diagnostics) {
        if (compiler_options.no_warnings && d.level != Level::Error) {
            continue;
        }
        if (compiler_options.error_format == "human") {
            out += render_diagnostic_human(d, input, lm,
                compiler_options.use_colors,
                compiler_options.show_stacktrace);
            if (&d != &this->diagnostics.back()) out += "\n";
        } else if (compiler_options.error_format == "short") {
            out += render_diagnostic_short(d, input, lm);
        } else {
            throw LCompilersException("Error format not supported.");
        }
    }
    if (compiler_options.error_format == "human") {
        if (this->diagnostics.size() > 0 && !compiler_options.no_error_banner) {
            if (!compiler_options.no_warnings || has_error()) {
                std::string bold  = "\033[0;1m";
                std::string reset = "\033[0;00m";
                if (!compiler_options.use_colors) {
                    bold = "";
                    reset = "";
                }
                out += "\n\n";
                out += bold + "Note" + reset
                    + ": if any of the above error or warning messages are not clear or are lacking\n";
                out += "context please report it to us (we consider that a bug that needs to be fixed).\n";
            }
        }
    }
    return out;
}

std::string get_line(std::string str, int n)
{
    std::string line;
    std::stringstream s(str);
    for (int i=0; i < n; i++) {
        std::getline(s, line);
    }
    return line;
}

void populate_span(diag::Span &s, const LocationManager &lm,
        const std::string &input) {
    lm.pos_to_linecol(lm.output_to_input_pos(s.loc.first, false),
        s.first_line, s.first_column);
    lm.pos_to_linecol(lm.output_to_input_pos(s.loc.last, true),
        s.last_line, s.last_column);
    s.filename = lm.in_filename;
    for (uint32_t i = s.first_line; i <= s.last_line; i++) {
        s.source_code.push_back(get_line(input, i));
    }
    LFORTRAN_ASSERT(s.source_code.size() > 0)
}

// Loop over all labels and their spans, populate all of them
void populate_spans(diag::Diagnostic &d, const LocationManager &lm,
        const std::string &input) {
    for (auto &l : d.labels) {
        for (auto &s : l.spans) {
            populate_span(s, lm, input);
        }
    }
}

// Fills Diagnostic with span details and renders it
std::string render_diagnostic_human(Diagnostic &d, const std::string &input,
        const LocationManager &lm, bool use_colors, bool show_stacktrace) {
    std::string out;
    if (show_stacktrace) {
        out += error_stacktrace(d.stacktrace);
    }
    // Convert to line numbers and get source code strings
    populate_spans(d, lm, input);
    // Render the message
    out += render_diagnostic_human(d, use_colors);
    return out;
}

// Fills Diagnostic with span details and renders it
std::string render_diagnostic_short(Diagnostic &d, const std::string &input,
        const LocationManager &lm) {
    std::string out;
    // Convert to line numbers and get source code strings
    populate_spans(d, lm, input);
    // Render the message
    out += render_diagnostic_short(d);
    return out;
}

std::string render_diagnostic_human(const Diagnostic &d, bool use_colors) {
    std::string bold  = "\033[0;1m";
    std::string red_bold  = "\033[0;31;1m";
    std::string yellow_bold  = "\033[0;33;1m";
    std::string green_bold  = "\033[0;32;1m";
    std::string blue_bold  = "\033[0;34;1m";
    std::string reset = "\033[0;00m";
    if (!use_colors) {
        bold = "";
        red_bold = "";
        yellow_bold = "";
        green_bold = "";
        blue_bold = "";
        reset = "";
    }
    std::stringstream out;

    std::string message_type = "";
    std::string primary_color = "";
    std::string type_color = "";
    switch (d.level) {
        case (Level::Error):
            primary_color = red_bold;    
            type_color = primary_color;
            switch (d.stage) {
                case (Stage::CPreprocessor):
                    message_type = "C preprocessor error";
                    break;
                case (Stage::Prescanner):
                    message_type = "prescanner error";
                    break;
                case (Stage::Tokenizer):
                    message_type = "tokenizer error";
                    break;
                case (Stage::Parser):
                    message_type = "syntax error";
                    break;
                case (Stage::Semantic):
                    message_type = "semantic error";
                    break;
                case (Stage::ASRPass):
                    message_type = "ASR pass error";
                    break;
                case (Stage::ASRVerify):
                    message_type = "ASR verify pass error";
                    break;
                case (Stage::CodeGen):
                    message_type = "code generation error";
                    break;
            }
            break;
        case (Level::Warning):
            primary_color = yellow_bold;    
            type_color = primary_color;
            message_type = "warning";
            break;
        case (Level::Note):
            primary_color = bold;    
            type_color = primary_color;
            message_type = "note";
            break;
        case (Level::Help):
            primary_color = bold;    
            type_color = primary_color;
            message_type = "help";
            break;
        case (Level::Style):
            primary_color = green_bold;
            type_color = yellow_bold;
            message_type = "style suggestion";
            break;
    }

    out << type_color << message_type << reset << bold << ": " << d.message << reset << std::endl;

    if (d.labels.size() > 0) {
        Label l = d.labels[0];
        Span s = l.spans[0];
        int line_num_width = 1;
        if (s.last_line >= 10000) {
            line_num_width = 5;
        } else if (s.last_line >= 1000) {
            line_num_width = 4;
        } else if (s.last_line >= 100) {
            line_num_width = 3;
        } else if (s.last_line >= 10) {
            line_num_width = 2;
        }
        // TODO: print the primary line+column here, not the first label:
        out << std::string(line_num_width, ' ') << blue_bold << "-->" << reset << " " << s.filename << ":" << s.first_line << ":" << s.first_column;
        if (s.first_line != s.last_line) {
            out << " - " << s.last_line << ":" << s.last_column;
        }
        out << std::endl;
        for (auto &l : d.labels) {
            if (l.spans.size() == 0) {
                throw LCompilersException("ICE: Label does not have a span");
            }
            std::string color;
            char symbol;
            if (l.primary) {
                color = primary_color;
                symbol = '^';
            } else {
                color = blue_bold;
                symbol = '~';
            }
            Span s0 = l.spans[0];
            for (size_t i=0; i < l.spans.size(); i++) {
                Span s2=l.spans[i];
                // If the span is on the same line as the last span and to
                // the right, we add it to the same line. Otherwise we start
                // a new line.
                if (i >= 1) {
                    if (s0.first_line == s0.last_line) {
                        // Previous span was single line
                        if (s2.first_line == s2.last_line && s2.first_line == s0.first_line)  {
                            // Current span is single line and on the same line
                            if (s2.first_column > s0.last_column+1) {
                                // And it comes after the previous span
                                // Append the span and continue
                                out << std::string(s2.first_column-s0.last_column-1, ' ');
                                out << std::string(s2.last_column-s2.first_column+1, symbol);
                                s0 = s2;
                                continue;
                            }
                        }
                        // Otherwise finish the line
                        out << " " << l.message << reset << std::endl;
                    }
                }
                // and start a new one:
                s0 = s2;
                if (s0.first_line == s0.last_line) {
                    out << std::string(line_num_width+1, ' ') << blue_bold << "|"
                        << reset << std::endl;
                    std::string line = s0.source_code[0];
                    std::replace(std::begin(line), std::end(line), '\t', ' ');
                    out << blue_bold << std::setw(line_num_width)
                        << std::to_string(s0.first_line) << " |" << reset << " "
                        << line << std::endl;
                    out << std::string(line_num_width+1, ' ') << blue_bold << "|"
                        << reset << " ";
                    out << std::string(s0.first_column-1, ' ');
                    out << color << std::string(s0.last_column-s0.first_column+1, symbol);
                } else {
                    if (s0.first_line < s0.last_line) {
                        out << std::string(line_num_width+1, ' ') << blue_bold << "|"
                            << reset << std::endl;
                        std::string line = s0.source_code[0];
                        std::replace(std::begin(line), std::end(line), '\t', ' ');
                        out << blue_bold << std::setw(line_num_width)
                            << std::to_string(s0.first_line) << " |" << reset << " "
                            << "   " + line << std::endl;
                        out << std::string(line_num_width+1, ' ') << blue_bold << "|"
                            << reset << " ";
                        out << "   " + std::string(s0.first_column-1, ' ');
                        int64_t repeat = (int64_t)line.size()-(int64_t)s0.first_column+1;
                        if (repeat > 0) {
                            out << color << std::string(repeat, symbol);
                        }
                        out << "..." << reset << std::endl;

                        out << "..." << std::endl;

                        out << std::string(line_num_width+1, ' ') << blue_bold << "|"
                            << reset << std::endl;
                        line = s0.source_code[s0.source_code.size()-1];
                        std::replace(std::begin(line), std::end(line), '\t', ' ');
                        out << blue_bold << std::setw(line_num_width)
                            << std::to_string(s0.last_line) << " |" << reset << " "
                            << "   " + line << std::endl;
                        out << std::string(line_num_width+1, ' ') << blue_bold << "|"
                            << reset << " ";
                        out << color << "..." + std::string(s0.last_column-1+1, symbol);
                        out << " " << l.message << reset << std::endl;
                    } else {
                        throw LCompilersException("location last_line < first_line");
                    }
                }
            }
            if (s0.first_line == s0.last_line) {
                out << " " << l.message << reset << std::endl;
            }
        } // Labels
    }
    return out.str();
}

std::string render_diagnostic_short(const Diagnostic &d) {
    std::stringstream out;

    std::string message_type = "";
    switch (d.level) {
        case (Level::Error):
            switch (d.stage) {
                case (Stage::CPreprocessor):
                    message_type = "C preprocessor error";
                    break;
                case (Stage::Prescanner):
                    message_type = "prescanner error";
                    break;
                case (Stage::Tokenizer):
                    message_type = "tokenizer error";
                    break;
                case (Stage::Parser):
                    message_type = "syntax error";
                    break;
                case (Stage::Semantic):
                    message_type = "semantic error";
                    break;
                case (Stage::ASRPass):
                    message_type = "ASR pass error";
                    break;
                case (Stage::ASRVerify):
                    message_type = "ASR verify pass error";
                    break;
                case (Stage::CodeGen):
                    message_type = "code generation error";
                    break;
            }
            break;
        case (Level::Warning):
            message_type = "warning";
            break;
        case (Level::Note):
            message_type = "note";
            break;
        case (Level::Help):
            message_type = "help";
            break;
        case (Level::Style):
            message_type = "style suggestion";
            break;
    }

    if (d.labels.size() > 0) {
        Label l = d.labels[0];
        Span s = l.spans[0];
        // TODO: print the primary line+column here, not the first label:
        out << s.filename << ":" << s.first_line << ":" << s.first_column;
        if (s.first_line != s.last_line) {
            out << " - " << s.last_line << ":" << s.last_column;
        }
        out << " ";
    }

    out << message_type << ": " << d.message << std::endl;

    return out.str();
}

} // namespace LFortran::diag
