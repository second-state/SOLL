[//]: # (SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception)
# Feature Guide for Yul

## Introduction

In this guide, we will list all Yul language features and briefly summarize the current status and limitations about SOLL implementation.

## Grammar

Our parser already covered most parts of valid [grammar](https://solidity.readthedocs.io/en/v0.6.1/yul.html#specification-of-yul) base on 0.6.1. Your can use `soll -lang=Yul -action=ASTDump ...*.yul` verify parser functional behavior.

If you want to execute ewasm compiled from Yul, refer to the [DevGuide](DevGuide.md).

### Supported by SOLL

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
    - Leave
- Basic arithmetical, compare and storage/memory access built-in functions.
- Partial state, object and misc built-in functions.

### Future works

Below will describe some under implement language features.

|                                             feature                                              |                           item                           |         exmaple         |
|--------------------------------------------------------------------------------------------------|----------------------------------------------------------|-------------------------|
| statement                                                                                        | Switch with string literal                               | case ""                 |
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
|                                                                                                  | selfdestruct                                             |                         |
|                                                                                                  | blockhash                                                |                         |
|                                                                                                  | balance                                                  |                         |
|                                                                                                  | this                                                     |                         |
|                                                                                                  | calldatacopy                                             |                         |
|                                                                                                  | codesize                                                 |                         |
|                                                                                                  | extcodesize                                              |                         |
|                                                                                                  | extcodecopy                                              |                         |
|                                                                                                  | extcodehash                                              |                         |
|                                                                                                  | \*datasize (see below)                                    |                         |
|                                                                                                  | \*dataoffset (see below)                                  |                         |
|                                                                                                  | discard                                                  |                         |
|                                                                                                  | discardu256                                              |                         |
|                                                                                                  | splitu256tou64                                           |                         |
|                                                                                                  | combineu64tou256                                         |                         |

> `datasize` and `dataoffset` in our implementation have a constraint.  
> They both couldn't be used apply on object itself.
