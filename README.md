# Compiler Visualizer

A web-based compiler toolkit that demonstrates core concepts of compiler design, including lexical analysis, LL(1) parsing, and LR(1) parsing with DFA visualization.

---

## 🚀 Features

### 🔹 Lexical Analyzer

* Tokenizes input source code (subset of C language)
* Identifies:

  * Keywords
  * Identifiers
  * Operators
  * Delimiters
  * Numbers
  * Strings and characters
  * Comments
* Maintains symbol tables for identifiers and constants
* Outputs structured token data in JSON format

---

### 🔹 LL(1) Parser

* Constructs FIRST and FOLLOW sets
* Builds predictive parsing table
* Performs top-down parsing using stack-based approach
* Displays:

  * Parsing steps
  * Parse table
  * Parse tree visualization

---

### 🔹 LR(1) Parser

* Implements bottom-up parsing (shift-reduce parsing)
* Builds canonical collection of LR(1) items
* Generates ACTION and GOTO tables
* Supports:

  * DFA-based state transitions
  * Conflict handling
* Provides step-by-step parsing visualization

---

### 🔹 DFA Visualization

* Generates DFA graphs for LR(1) item sets
* Uses Graphviz (DOT format) for rendering
* Supports:

  * Web visualization
  * Export as PDF

---

## 🧠 Architecture

```
Frontend (HTML + JavaScript)
        ↓
PHP Backend (API layer)
        ↓
C++ Core Engine (Lexer + Parsers)
        ↓
JSON Output → UI Rendering
```

---

## ⚙️ Tech Stack

* **C++** → Core compiler logic
* **PHP** → Backend communication
* **HTML/CSS/JavaScript** → Frontend UI
* **Graphviz** → DFA visualization

---

## 📌 Key Concepts Implemented

* Tokenization using finite automata
* FIRST & FOLLOW set computation
* Predictive parsing (LL(1))
* Shift-reduce parsing (LR(1))
* DFA construction for LR parsing
* Stack-based parsing simulation

---

## ▶️ How to Run

1. Compile C++ modules:

```
make
```

2. Start a local server (e.g., Apache / PHP server)

3. Open the main page in browser:

```
http://localhost/
```

---

## 🎯 Learning Outcomes

This project demonstrates:

* Understanding of compiler design fundamentals
* Implementation of parsing algorithms
* Integration of multiple technologies (C++, PHP, JS)
* Visualization of complex theoretical concepts

---

## 📷 Demo

* Lexical Analyzer
* LL(1) Parser
* LR(1) Parser
* DFA Graph Visualization

---

## 📌 Note

This project is built for educational purposes to explore compiler design concepts and their practical implementation.
