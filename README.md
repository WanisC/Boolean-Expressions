# Boolean Expressions Evaluation Project

## Authors
- Wanis CHOUAIB
- Elie KANGA
## Instructors
- Franck Quesette
  - Email: franck.quessette@uvsq.fr
- Pierre COUCHENEY
  - Email: pierre.coucheney@uvsq.fr
- Yann Strozecki
  - Email: yann.strozecki@uvsq.fr

## Overview

The aim of this project is to develop an automaton that recognizes a language defined by a regular expression and to test word membership using the automaton. The project is divided into two main parts:

### Part 1: Lexical Analyzer

In this phase, we will create a lexical analyzer to recognize the various tokens required for the application. This involves working with the `regexp.l` file.

### Part 2: Axioms and Automaton Construction

In the second part, we define our axioms and their production rules in the `regexp.y` file. This step allows us to recognize the regular expression and build our automaton. Additionally, we define the words that need testing and generate a main file that will be compiled.

The ε-transition removal and determinization methods are implemented to simplify the construction of the word recognition method.

## How to Use

To download and set up the project, follow these steps:

1. Run the following command to update the project:
