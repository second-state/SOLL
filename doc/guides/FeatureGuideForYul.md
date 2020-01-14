[//]: # (SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception)
# Feature Guide for Yul

## Introduction

In this guide, we will list all Yul language features and briefly summarize the current status and limitations about SOLL implementation.

## Grammar

Our parser already covered most parts of valid [grammar](https://solidity.readthedocs.io/en/v0.6.1/yul.html#specification-of-yul) base on 0.6.1. Your can use `soll -lang=Yul -action=ASTDump ...*.yul` verify parser functional behavior.

If you want to execute ewasm compiled from Yul, refer to the [DevGuide](DevGuide.md).

- Recognize Yul object structure.
- Statement
    - Block
    - FunctionDefinition
    - VariableDeclaration
    - Assignment
    - Switch
    - If
    - ForLoop
    - BreakContinue
    - FunctionCall
- Basic arithmetical, compare, storage and memory access built-in functions.

Below will describe some under implement language features. 

|                                             feature                                              |                           item                           |         exmaple         |
|--------------------------------------------------------------------------------------------------|----------------------------------------------------------|-------------------------|
| statement                                                                                        | Nested object                                            |                         |
|                                                                                                  | Function return multiple value                           | function f() -> x, y {} |
|                                                                                                  | Declare multiple variables in single statement           | let a, b                |
|                                                                                                  | Assign with multiple value in single statement           | a, b = func()           |
|                                                                                                  | Switch with string literal                               | case ""                 |
|                                                                                                  | Leave statement **0.6.0 feature**                        |                         |
| [Yul built-in function](https://solidity.readthedocs.io/en/v0.5.12/yul.html#low-level-functions) | logic not/and/or/xor <br>(conflict with inline assembly) |                         |
|                                                                                                  | signextendu256                                           |                         |
|                                                                                                  | sars256                                                  |                         |
|                                                                                                  | byte                                                     |                         |
|                                                                                                  | create                                                   |                         |
|                                                                                                  | create2                                                  |                         |
|                                                                                                  | call                                                     |                         |
|                                                                                                  | callcode                                                 |                         |
|                                                                                                  | delegatecall                                             |                         |
|                                                                                                  | abort                                                    |                         |
|                                                                                                  | return                                                   |                         |
|                                                                                                  | selfdestruct                                             |                         |
|                                                                                                  | log0                                                     |                         |
|                                                                                                  | log1                                                     |                         |
|                                                                                                  | log2                                                     |                         |
|                                                                                                  | log3                                                     |                         |
|                                                                                                  | log4                                                     |                         |
|                                                                                                  | blockhash                                                |                         |
|                                                                                                  | balance                                                  |                         |
|                                                                                                  | this                                                     |                         |
|                                                                                                  | caller                                                   |                         |
|                                                                                                  | callvalue                                                |                         |
|                                                                                                  | calldatasize                                             |                         |
|                                                                                                  | calldatacopy                                             |                         |
|                                                                                                  | codesize                                                 |                         |
|                                                                                                  | codecopy                                                 |                         |
|                                                                                                  | extcodesize                                              |                         |
|                                                                                                  | extcodecopy                                              |                         |
|                                                                                                  | extcodehash                                              |                         |
|                                                                                                  | datasize                                                 |                         |
|                                                                                                  | dataoffset                                               |                         |
|                                                                                                  | datacopy                                                 |                         |
|                                                                                                  | discard                                                  |                         |
|                                                                                                  | discardu256                                              |                         |
|                                                                                                  | splitu256tou64                                           |                         |
|                                                                                                  | combineu64tou256                                         |                         |
