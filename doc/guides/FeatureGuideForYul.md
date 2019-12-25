[//]: # (SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception)
# Feature Guide for Yul

## Introduction

In this guide, we will list all Yul language features and briefly summarize the current status and limitations about SOLL implementation.

## Grammar

Our parser already covered most parts of valid [grammar](https://solidity.readthedocs.io/en/v0.5.12/yul.html#specification-of-yul). Your can use `soll -lang=Yul -action=ASTDump ...*.yul` verify parser functional behavior. Unfortunately, SOLL doesn't provide diagnostic tool to report invalid grammar for the Yul language in this stage.

If you want to execute ewasm compiled from Yul, refer to the [DevGuide](DevGuide.md).

- Recognize Yul object structure.
- Statement
    - Block
    - FunctionDefinition
    - VariableDeclaration
    - Assignment
    - If
    - ForLoop
    - BreakContinue
- Basic arithmetical, compare, sstore and sload built-in functions.

Below will describe some under implement language features. 

|                                           feature                                            |                  item                 |
|----------------------------------------------------------------------------------------------|---------------------------------------|
| statement                                                                                    | Declare an object inside other object |
|                                                                                              | Call user define function             |
|                                                                                              | Switch statement                      |
| [built-in function](https://solidity.readthedocs.io/en/v0.5.12/yul.html#low-level-functions) | signextendu256                        |
|                                                                                              | sars256                               |
|                                                                                              | byte                                  |
|                                                                                              | mload                                 |
|                                                                                              | mstore                                |
|                                                                                              | mstore8                               |
|                                                                                              | msize                                 |
|                                                                                              | create                                |
|                                                                                              | create2                               |
|                                                                                              | call                                  |
|                                                                                              | callcode                              |
|                                                                                              | delegatecall                          |
|                                                                                              | abort                                 |
|                                                                                              | return                                |
|                                                                                              | selfdestruct                          |
|                                                                                              | log0                                  |
|                                                                                              | log1                                  |
|                                                                                              | log2                                  |
|                                                                                              | log3                                  |
|                                                                                              | log4                                  |
|                                                                                              | blockhash                             |
|                                                                                              | balance                               |
|                                                                                              | this                                  |
|                                                                                              | caller                                |
|                                                                                              | callvalue                             |
|                                                                                              | calldataload                          |
|                                                                                              | calldatasize                          |
|                                                                                              | calldatacopy                          |
|                                                                                              | codesize                              |
|                                                                                              | codecopy                              |
|                                                                                              | extcodesize                           |
|                                                                                              | extcodecopy                           |
|                                                                                              | extcodehash                           |
|                                                                                              | datasize                              |
|                                                                                              | dataoffset                            |
|                                                                                              | datacopy                              |
|                                                                                              | discard                               |
|                                                                                              | discardu256                           |
|                                                                                              | splitu256tou64                        |
|                                                                                              | combineu64tou256                      |