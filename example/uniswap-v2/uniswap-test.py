import ctypes
from binascii import unhexlify
from evmc import *
from utils import *
import os
import signal
import glob
import re

handle = ctypes.cdll.LoadLibrary('./libsoll_runtime_test.so')


def evmc_vm_execute(
        calldata: bytes,
        sender: int,
        destination: int,
        wasm: bytes):
    _evmc_vm_execute = handle.evmc_vm_execute

    sender = int_to_address(sender)
    destination = int_to_address(destination)

    result = struct_evmc_result()
    _evmc_vm_execute(
        ctypes.cast(calldata, ctypes.c_char_p),
        len(calldata),
        ctypes.pointer(sender),
        ctypes.pointer(destination),
        ctypes.cast(wasm, ctypes.c_char_p),
        len(wasm),
        ctypes.pointer(result))

    return evmc_status_code__enumvalues[result.status_code], result


def evmc_vm_deploy(deploy_wasm: bytes):
    _evmc_vm_deploy = handle.evmc_vm_deploy

    result = struct_evmc_result()
    _evmc_vm_deploy(ctypes.cast(deploy_wasm, ctypes.c_char_p),
                    len(deploy_wasm),
                    ctypes.pointer(result))

    return evmc_status_code__enumvalues[result.status_code], result


def evmc_get_storage(address: int, key: int):
    _evmc_get_storage = handle.evmc_get_storage

    address = int_to_address(address)
    key = int_to_bytes32(key)
    result = int_to_bytes32(0)

    _evmc_get_storage(
        ctypes.pointer(address),
        ctypes.pointer(key),
        ctypes.pointer(result))

    return bytes(result.raw)


wasm = open(f'UniswapV2ERC20.wasm', 'rb').read()

def tester(name, calldata, sender, ret_is_byte=False ,wasm=wasm):
    status, result = evmc_vm_execute(
            unhexlify(calldata),
            sender,
            0x0,
            wasm
    )

    if result.output_size > 0:
        output = ctypes.cast(result.output_data, ctypes.POINTER(ctypes.c_char * result.output_size))
        ret = bytes(output.contents)
        if not ret_is_byte:
            ret = bytes_to_long(ret)
        print(name)
        print("status: ", status)
        print("return: ", ret)

    return (status == 'EVMC_SUCCESS')


erc20_tests = [
# // 0x06fdde03 name() ABI signature
    ("name", 
      b'06fdde03'.ljust(64, b'0'),
      0x7fffffff,
      True
    ),
# // 0x95d89b41 symbol() ABI signature
    ("symbol", 
      b'95d89b41'.ljust(64, b'0'),
      0x7fffffff,
      True
    ),
# // 0x313ce567 decimals() ABI signature
    ("decimals", 
      b'313ce567'.ljust(64, b'0'),
      0x7fffffff,
      False
    ),
# // 0x18160ddd totalSupply() ABI signature
    ("total_supply", 
      b'18160ddd'.ljust(64, b'0'),
      0x7fffffff,
      False
    ),
# // 0x40c10f19 mint(address, 0x3) ABI signature
    ("mint_0xDEADBEEF_3", 
      b'40c10f19'.ljust(64, b'0')
         + b'DEADBEEF'.ljust(64, b'0')
         + b'00000003'.ljust(64, b'0'),
      0xDEADBEEF,
      False 
    ),
# // 0x18160ddd totalSupply() ABI signature
    ("total_supply", 
      b'18160ddd'.ljust(64, b'0'),
      0x7fffffff,
      False 
    ),
# // 0x9993021a do_balance(address) ABI signature
    ("balance_of_0xDEADBEEF", 
      b'70a08231'.ljust(64, b'0')
         + b'DEADBEEF'.ljust(64, b'0'),
      0xDEADBEEF,
      False 
    ),
# // 0x9993021a do_balance(address) ABI signature
    ("balance_of_0xFACEB00C", 
      b'70a08231'.ljust(64, b'0')
         + b'FACEB00C'.ljust(64, b'0'),
      0xDEADBEEF,
      False #0xff
    ),
# // 0xa9059cbb transfer() ABI signature
    ("transfer_0xDEADBEEF_1_to_0xFACEB00C", 
      b'a9059cbb'.ljust(64, b'0')
         + b'FACEB00C'.ljust(64, b'0')
         + b'00000001'.ljust(64, b'0'),
      0xDEADBEEF,
      False 
    ),     
# // 0x70a08231 do_balance(address) ABI signature
    ("balance_of_0xDEADBEEF", 
      b'70a08231'.ljust(64, b'0')
         + b'DEADBEEF'.ljust(64, b'0'),
      0xDEADBEEF,
      False 
    ),
# // 0x70a08231 do_balance(address) ABI signature
    ("balance_of_0xFACEB00C", 
      b'70a08231'.ljust(64, b'0')
         + b'FACEB00C'.ljust(64, b'0'),
      0xDEADBEEF,
      False
    ),
# // 0x18160ddd totalSupply() ABI signature
    ("total_supply", 
      b'18160ddd'.ljust(64, b'0'),
      0x7fffffff,
      False 
    ),
# // 0xa9059cbb transfer() ABI signature
    ("transfer_0xDEADBEEF_5_to_0xFACEB00C", 
      b'a9059cbb'.ljust(64, b'0')
         + b'FACEB00C'.ljust(64, b'0')
         + b'00000005'.ljust(64, b'0'),
      0xDEADBEEF,
      False 
    ),     
]

for test, hexstr, sender, ret_is_byte in erc20_tests:
    tester(test, hexstr, sender, ret_is_byte)
    print("=====================================")


