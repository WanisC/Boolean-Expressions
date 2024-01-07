# Boolean Expressions Evaluation Project

## Authors
- Wanis CHOUAIB
  wanis.chouaib@ens.uvsq.fr
- Elie KANGA
  elie.kanga@ens.uvsq.fr

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
  ```bash
   make update
   ```
   
2. Install the required dependencies using:
  ```bash
   make install
   ```

## Building and Compilation

To compile project, execute the following command:
```bash
make run
```
This will build the necessary components and execute also the compiled file for the automaton.
_Note that using make alone works just as well_

## Instructors
- Franck Quesette
  franck.quessette@uvsq.fr
- Pierre COUCHENEY
  pierre.coucheney@uvsq.fr
- Yann Strozecki
  yann.strozecki@uvsq.fr

For any inquiries, please contact the respective authors via their provided email addresses. 

## Version

Current version: v2023-2024

## Additional Information

* Langage: C
* Teaching unit: Language Theory
* University: UVSQ-Versailles
