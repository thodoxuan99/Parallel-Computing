#include <iostream>
#include <fstream>
#include <stdexcept>

#include <string>

#include "../include/graph.h"
#include "../include/util.h"

// Read and store a graph from a file specified by the argument file name and its format.
void Graph::ReadFromFile(const std::string &filename, const std::string &format) {
    std::ifstream f;
    std::string line;
    std::vector<std::string> tokens;
    char err_buf[64];

    if (format.compare("DIMACS") == 0) {
        f.open(filename, std::ifstream::in);
        // Scan through the file and look for the problem line.
        bool problem_read = false;
        for (int i = 1; std::getline(f, line) && !problem_read; ++i) {
            if (tokens[0].compare("p") == 0) {
                // problem line
                if (problem_read) {
                    sprintf(err_buf, "%s:%d: miltiple problem lines found", filename.c_str(), i);
                    throw std::runtime_error(err_buf);
                }
                if (tokens.size() != 4) {
                    sprintf(err_buf, "%s:%d: invalid number of tokens", filename.c_str(), i);
                    throw std::length_error(err_buf);
                }
                n_nodes_ = std::stoi(tokens[2]);
                n_edges_ = std::stoi(tokens[3]);
                problem_read = true;
            } else if (tokens[0].compare("a") == 0) {
                // edge descriptor; shouldn't appear before finding a problem line
                sprintf(err_buf, "%s:%d: edge read before the problem", filename.c_str(), i);
                throw std::logic_error(err_buf);
            }
        }
        if (!problem_read) {
            sprintf(err_buf, "%s: missing the problem line", filename.c_str());
            throw std::runtime_error(err_buf);
        }
        // Start building the sparse matrix using the problem line in the file.

        std::vector<int> tmp_ia(n_nodes_ + 1, 0);    // temporary container for IA


        // Scan the file again and construct the graph.
        for (int i = 1; std::getline(f, line); ++i) {
            Split(line, " ", tokens);
            if (tokens[0].compare("c") == 0) {
                // comment; do nothing
            } else if (tokens[0].compare("a") == 0) {
                // edge descriptor
                if (tokens.size() != 4) {
                    sprintf(err_buf, "%s:%d: invalid number of tokens", filename.c_str(), i);
                    throw std::length_error(err_buf);
                }
                // DIMACS already uses 1 based indexing; since IA's first element
                // is a placeholder, this token can be used directly.
                tmp_ia[std::stoi(tokens[1])]++;

            } else {
                sprintf(err_buf, "%s:%d: line starts with unknown token", filename.c_str(), i);
                throw std::invalid_argument(err_buf);
            }
            tokens.clear();
        }
        f.close();

        nodes_ = std::vector<double>(n_nodes_, 0.0);     

    } else {
        // Handle invalid file formats.
        sprintf(err_buf, "%s: invalid file format `%s`", filename.c_str(), format.c_str());
        throw std::invalid_argument(err_buf);
    }
}