# -*- Python -*-

import os

import lit.formats

from lit.llvm import llvm_config
from lit.llvm.subst import ToolSubst
from lit.llvm.subst import FindTool

# Configuration file for the 'lit' test runner.

# name: The name of this test suite.
config.name = 'SOLL'

# testFormat: The test format to use to interpret tests.
#
# For now we require '&&' between commands, until they get globally killed and
# the test runner updated.
config.test_format = lit.formats.ShTest(not llvm_config.use_lit_shell)

# suffixes: A list of file extensions to treat as test files.
config.suffixes = [
    '.sol', '.yul'
]

config.available_features = [
    'address',
    'balance',
    'codesize',
    'extcodesize',
    'blockhash',
    'codecopy',
    'extcodecopy',
    'returndatacopy',
    'mload',
    'mstore',
    'mstore8',
    'msize',
    'sload',
    'sstore',
    'stop',
    'return',
    'revert',
    'log0',
    'log1',
    'log2',
    'log3',
    'log4',
    'coinbase',
    'difficulty',
    'gaslimit',
    'number',
    'timestamp',
    'origin',
    'gasprice',
    'gas',
    'call',
    'callcode',
    'delegatecall',
    'staticcall',
    'caller',
    'callvalue',
    'calldataload',
    'calldatasize',
    'calldatacopy',
    'codecopy',
    'datasize',
    'dataoffset',
    'datacopy',
    'leave',
    'declareMulti',
    'returnTuple',
    'returndatasize',
    'objectInObject',
    'implicitCast',
    'invalid',
    'namedcall'
]

# excludes: A list of directories to exclude from the testsuite.
config.excludes = [
    'CMakeLists.txt'
]

# test_source_root: The root path where tests are located.
config.test_source_root = os.path.dirname(__file__)

# test_exec_root: The root path where tests should be run.
config.test_exec_root = os.path.join(config.soll_obj_root, 'test')

llvm_config.use_default_substitutions()

tool_substitutions = [
    ToolSubst('%soll', command=config.soll, extra_args=[]),
]
llvm_config.add_tool_substitutions(tool_substitutions)

# For each occurrence of a soll tool name, replace it with the full path to
# the build directory holding that tool. We explicitly specify the directories
# to search to ensure that we get the tools just built and not some random
# tools that might happen to be in the user's PATH.
tool_dirs = [config.soll_tools_dir, config.llvm_tools_dir]

tools = [
    'opt',
    ToolSubst('%soll_extdef_map',
              command=FindTool('soll-extdef-mapping'),
              unresolved='ignore'),
]

llvm_config.add_tool_substitutions(tools, tool_dirs)
