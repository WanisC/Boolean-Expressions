Boolean Expressions Evaluation Project
Authors
Wanis CHOUAIB
Elie KANGA
Franck Quesette
Email: franck.quessette@uvsq.fr
Pierre COUCHENEY
Email: pierre.coucheney@uvsq.fr
Yann Strozecki
Email: yann.strozecki@uvsq.fr
Overview
The aim of this project is to develop an automaton that recognizes a language defined by a regular expression and to test word membership using the automaton. The project is divided into two main parts:

Part 1: Lexical Analyzer
In this phase, we will create a lexical analyzer to recognize the various tokens required for the application. This involves working with the regexp.l file.

Part 2: Axioms and Automaton Construction
In the second part, we define our axioms and their production rules in the regexp.y file. This step allows us to recognize the regular expression and build our automaton. Additionally, we define the words that need testing and generate a main file that will be compiled.

The ε-transition removal and determinization methods are implemented to simplify the construction of the word recognition method.

How to Use
To download and set up the project, follow these steps:

Run the following command to update the project:

go
Copy code
make update
Install the required dependencies using:

go
Copy code
make install
Building and Compilation
To compile the project, execute the following commands:

bash
Copy code
make
This will build the necessary components and generate the executable for the automaton.

Usage
After compilation, run the application using:

bash
Copy code
./your_executable_name
Contributors
Wanis CHOUAIB
Elie KANGA
Franck Quesette
Pierre COUCHENEY
Yann Strozecki
For any inquiries, please contact the respective authors via their provided email addresses.

Version
Current version: v2023-2024

Additional Information
Language: C
Dependencies: (list any dependencies)
License: (specify the project's license, if applicable)
Feel free to customize this README to suit your project's specific details and requirements.
