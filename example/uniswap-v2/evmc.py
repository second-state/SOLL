import ctypes

if ctypes.sizeof(ctypes.c_void_p) == 8:
    POINTER_T = ctypes.POINTER
else:
    import _ctypes
    ctypes._pointer_t_type_cache = {}

    def POINTER_T(pointee):
        fake_ptr_base_type = ctypes.c_uint64
        if pointee is None:  # VOID pointer type. c_void_p.
            pointee = type(None)  # ctypes.c_void_p # ctypes.c_ulong
            clsname = 'c_void'
        else:
            clsname = pointee.__name__
        if clsname in ctypes._pointer_t_type_cache:
            return ctypes._pointer_t_type_cache[clsname]
        # make template

        class _T(_ctypes._SimpleCData,):
            _type_ = 'L'
            _subtype_ = pointee

            def _sub_addr_(self):
                return self.value

            def __repr__(self):
                return '%s(%d)' % (clsname, self.value)

            def contents(self):
                raise TypeError('This is not a ctypes pointer.')

            def __init__(self, **args):
                raise TypeError(
                    'This is not a ctypes pointer. It is not instanciable.')
        _class = type('LP_%d_%s' % (8, clsname), (_T,), {})
        ctypes._pointer_t_type_cache[clsname] = _class
        return _class

c_int128 = ctypes.c_ubyte * 16
c_uint128 = c_int128
void = None
if ctypes.sizeof(ctypes.c_longdouble) == 16:
    c_long_double_t = ctypes.c_longdouble
else:
    c_long_double_t = ctypes.c_ubyte * 16


def long_to_bytes(n):
    l = []
    x = 0
    off = 0
    while x != n:
        b = (n >> off) & 0xFF
        l.append(b)
        x = x | (b << off)
        off += 8
    l.reverse()
    return bytes(l)


def bytes_to_long(s):
    n = s[0]
    for b in (x for x in s[1:]):
        n = (n << 8) | b
    return n


def int_to_address(x):
    return ctypes.c_buffer(long_to_bytes(x).rjust(20, b'\x00'), size=20)


def int_to_bytes32(x):
    return ctypes.c_buffer(long_to_bytes(x).rjust(32, b'\x00'), size=32)


class struct_evmc_bytes32(ctypes.Structure):
    _pack_ = True  # source:False
    _fields_ = [
        ('bytes', ctypes.c_byte * 32)
    ]


struct_evmc_uint256be = struct_evmc_bytes32


class struct_evmc_address(ctypes.Structure):
    _pack_ = True  # source:False
    _fields_ = [
        ('bytes', ctypes.c_char * 20)
    ]


class struct_evmc_host_interface(ctypes.Structure):
    _pack_ = True  # source:False
    _fields_ = [
        ('account_exists', ctypes.c_void_p),
        ('get_storage', ctypes.c_void_p),
        ('set_storage', ctypes.c_void_p),
        ('get_balance', ctypes.c_void_p),
        ('get_code_size', ctypes.c_void_p),
        ('get_code_hash', ctypes.c_void_p),
        ('copy_code', ctypes.c_void_p),
        ('selfdestruct', ctypes.c_void_p),
        ('call', ctypes.c_void_p),
        ('get_tx_context', ctypes.c_void_p),
        ('get_block_hash', ctypes.c_void_p),
        ('emit_log', ctypes.c_void_p)
    ]


class struct_evmc_result(ctypes.Structure):
    _fields_ = [
        ('status_code', ctypes.c_int),
        ('gas_left', ctypes.c_int64),
        ('output_data', ctypes.POINTER(ctypes.c_int8)),
        ('output_size', ctypes.c_size_t),
        ('release', ctypes.c_void_p),
        ('create_address', struct_evmc_address),
        ('padding', ctypes.c_byte * 4),
    ]


class struct_evmc_host_context(ctypes.Structure):
    pass


class struct_evmc_message(ctypes.Structure):
    _pack_ = True  # source:False
    _fields_ = [
        ('kind', ctypes.c_int),
        ('flags', ctypes.c_uint32),
        ('depth', ctypes.c_int32),
        ('gas', ctypes.c_int64),
        ('destination', struct_evmc_address),
        ('sender', struct_evmc_address),
        ('input_data', ctypes.POINTER(ctypes.c_byte)),
        ('input_size', ctypes.c_size_t),
        ('value', struct_evmc_uint256be),
        ('create2_salt', struct_evmc_bytes32),
    ]


class struct_evmc_vm(ctypes.Structure):
    _pack_ = True  # source:False
    _fields_ = [
        ('abi_version', ctypes.c_int),
        ('name', ctypes.c_char_p),
        ('version', ctypes.c_char_p),
        ('destroy', ctypes.c_void_p),
        ('execute', ctypes.c_void_p),
        ('get_capabilities', ctypes.c_void_p),
        ('set_option', ctypes.c_void_p)
    ]


evmc_status_code__enumvalues = {
    0: 'EVMC_SUCCESS',
    1: 'EVMC_FAILURE',
    2: 'EVMC_REVERT',
    3: 'EVMC_OUT_OF_GAS',
    4: 'EVMC_INVALID_INSTRUCTION',
    5: 'EVMC_UNDEFINED_INSTRUCTION',
    6: 'EVMC_STACK_OVERFLOW',
    7: 'EVMC_STACK_UNDERFLOW',
    8: 'EVMC_BAD_JUMP_DESTINATION',
    9: 'EVMC_INVALID_MEMORY_ACCESS',
    10: 'EVMC_CALL_DEPTH_EXCEEDED',
    11: 'EVMC_STATIC_MODE_VIOLATION',
    12: 'EVMC_PRECOMPILE_FAILURE',
    13: 'EVMC_CONTRACT_VALIDATION_FAILURE',
    14: 'EVMC_ARGUMENT_OUT_OF_RANGE',
    15: 'EVMC_WASM_UNREACHABLE_INSTRUCTION',
    16: 'EVMC_WASM_TRAP',
    -1: 'EVMC_INTERNAL_ERROR',
    -2: 'EVMC_REJECTED',
    -3: 'EVMC_OUT_OF_MEMORY',
}
EVMC_SUCCESS = 0
EVMC_FAILURE = 1
EVMC_REVERT = 2
EVMC_OUT_OF_GAS = 3
EVMC_INVALID_INSTRUCTION = 4
EVMC_UNDEFINED_INSTRUCTION = 5
EVMC_STACK_OVERFLOW = 6
EVMC_STACK_UNDERFLOW = 7
EVMC_BAD_JUMP_DESTINATION = 8
EVMC_INVALID_MEMORY_ACCESS = 9
EVMC_CALL_DEPTH_EXCEEDED = 10
EVMC_STATIC_MODE_VIOLATION = 11
EVMC_PRECOMPILE_FAILURE = 12
EVMC_CONTRACT_VALIDATION_FAILURE = 13
EVMC_ARGUMENT_OUT_OF_RANGE = 14
EVMC_WASM_UNREACHABLE_INSTRUCTION = 15
EVMC_WASM_TRAP = 16
EVMC_INTERNAL_ERROR = -1
EVMC_REJECTED = -2
EVMC_OUT_OF_MEMORY = -3
evmc_status_code = ctypes.c_int  # enum
