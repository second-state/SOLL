import ctypes
from binascii import unhexlify
from evmc import *
from utils import *
import os
import signal
import glob

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


def testing(testbase):
    basic_sstore = open(f'{testbase}.wasm', 'rb').read()

    calldata = unhexlify("")
    sender = 0x0
    destination = 0x7ffffffff
    result_str, result = evmc_vm_execute(
        calldata, 0, destination, basic_sstore)
    if result_str == 'EVMC_SUCCESS':
        print_green(f"--- evmc vm execute result: {result_str}")
    else:
        print_red(f"--- evmc vm execute result: {result_str}")
        return -1, False

    storage_dump = dict()
    with open(f'{testbase}.yul', 'r') as f:
        storage_dump_flag = False
        for line in f:
            if storage_dump_flag:
                _, key, value = line.split()
                key, value = int(key[:-1], 16), int(value, 16)
                storage_dump[key] = value
            if line.find('Storage dump:') != -1:
                storage_dump_flag = True

    num_testcase = len(storage_dump)

    result = True
    for i, p in enumerate(storage_dump.items()):
        k, v = p
        execute_result = bytes_to_long(evmc_get_storage(destination, k))
        result = result and (v == execute_result)
        if (v == execute_result):
            print_green(f"testcase-{i}: PASS")
        else:
            print_red(f"testcase-{i}: FAIL")

    return num_testcase, result


if __name__ == '__main__':
    files = []
    files.extend(glob.glob('./libyul/*/*.yul'))
    files.extend(glob.glob('./yul/*/*.yul'))
    files.extend(glob.glob('./yul/*.yul'))

    num_test = 0
    num_run = 0
    num_has_testcase = 0
    num_pass = 0

    for file in files:
        print("========================================")
        base = file[:-4]
        print(base)
        if os.path.isfile(base + '.yul') and os.path.isfile(base + '.wasm'):
            num_testcase, result = testing(base)
            num_test += 1
            if num_testcase > 0:
                num_run += 1
                num_has_testcase += 1

                if (result):
                    num_pass += 1
                    print_green(f"--- storage check: PASS")
                else:
                    print_red(f"--- storage check: FAIL")
            elif num_testcase == 0:
                num_run += 1
                print(f"--- storage check: HAS NO TESTCASE")
            else:
                print_red(f"{base}: FAIL")

    print("pass rate (can run):", 100 * num_run / num_test, "%")
    print(
        "pass rate (storage correct):",
        100 *
        num_pass /
        num_has_testcase,
        "%")
