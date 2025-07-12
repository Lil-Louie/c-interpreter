<<<<<<< HEAD
🖥️ C Interpreter — Recursive Descent Parser & Shunting Yard Algorithm

Welcome to the C Interpreter Project, a powerful, lightweight interpreter designed for evaluating custom C-like programs. Built from scratch, this interpreter leverages advanced parsing techniques, including recursive descent parsing and the shunting yard algorithm for efficient expression evaluation.

🚀 Features

🔍 Recursive Descent Parser
A robust parsing engine that handles complex grammatical structures with ease, supporting expressions, conditionals, loops, and functions.
🧮 Shunting Yard Algorithm
Optimizes the evaluation of mathematical expressions, ensuring correct operator precedence and associativity.
🌳 Concrete Syntax Tree (CST) Representation
Utilizes an LCRS (Left-Child, Right-Sibling) binary tree structure to manage program syntax in an intuitive, memory-efficient format.
🏗️ Function Evaluation System
Supports custom functions, including a demonstration with sum_of_first_n_squares()—executed dynamically after evaluating the main() function.
🛠️ Modular Code Design
Easily extensible, making it simple to add new language features, operators, or syntax rules.
📚 How It Works

The interpreter follows a multi-stage process:

Lexical Analysis: Breaks down the source code into tokens.
Parsing: Uses recursive descent to build a syntax tree from the tokens.
Evaluation: Runs the program by traversing the CST and applying the shunting yard algorithm for expression resolution.
🔧 Build Instructions

Make sure you have CMake and a modern C++ compiler installed.

# Clone the repository
git clone https://github.com/yourusername/c-interpreter.git
cd c-interpreter

# Create a build directory
mkdir build && cd build

# Generate build files using CMake
cmake ..

# Compile the project
make

# Run the interpreter
./CS460_A6_Interpreter
📝 Usage Example

int sum_of_first_n_squares(int n) {
    int sum = 0;
    for (int i = 1; i <= n; i++) {
        sum = sum + (i * i);
    }
    return sum;
}

int main() {
    int result = sum_of_first_n_squares(5);
    print(result); // Output: 55
}
🧑‍💻 Contributing

Contributions are welcome! Feel free to fork the repository, submit pull requests, or suggest new features. If you find a bug, open an issue so we can squash it together.

📄 License

This project is licensed under the MIT License. See the LICENSE file for more information.

Developed by Luis Galvez, Xander, Christian Gonzalez, and Anthony Manese
Your feedback and suggestions are always appreciated! 🚀

=======
# c-interpreter
A simplified C interpreter written in C++ featuring tokenization, recursive descent parsing, AST/CST construction, and function execution.
>>>>>>> 4b82021d2643cbcdcd172609ddd3144f088b976b
