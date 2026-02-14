#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <functional>
#include <string>
#include "Core/RNG.h"

namespace Core {

// 1. Strict Integer-Only Types
using GVMInt = int64_t;

// 2. Constants
constexpr size_t MAX_STACK = 1024;
constexpr size_t MAX_GLOBALS = 256;
constexpr size_t MAX_GAS = 10000;

// 3. Opcodes (Instruction Set)
enum class Opcode : uint8_t {
    HALT = 0x00,
    NOP  = 0x01,
    
    // Stack Ops
    PUSH = 0x10, // Followed by 8-byte int
    POP  = 0x11,
    DUP  = 0x12,
    
    // Arithmetic (Wrap-Around)
    ADD  = 0x20,
    SUB  = 0x21,
    MUL  = 0x22,
    DIV  = 0x23, // Traps on zero
    MOD  = 0x24,
    ABS  = 0x25,
    NEG  = 0x26,
    
    // Comparison (Pushes 1 or 0)
    EQ   = 0x30,
    NEQ  = 0x31,
    LT   = 0x32,
    GT   = 0x33,
    LE   = 0x34,
    GE   = 0x35,
    
    // Logic
    AND  = 0x40,
    OR   = 0x41,
    NOT  = 0x42,
    
    // Control Flow
    JMP  = 0x50, // Unconditional Jump (Relative)
    JZ   = 0x51, // Jump if Zero
    JNZ  = 0x52, // Jump if Not Zero
    
    // Data Access
    LOAD = 0x60, // Load Global [Index]
    STORE= 0x61, // Store Global [Index]
    
    // External Interface
    EXT_CALL = 0xF0, // Call External Function -> ID
    
    // Debug
    PRINT = 0xFF
};

// 4. Execution Context
struct VMContext {
    std::array<GVMInt, MAX_STACK> stack{};
    std::array<GVMInt, MAX_GLOBALS> globals{};
    uint32_t sp = 0; // Stack Pointer
    uint32_t pc = 0; // Program Counter
    RNG* rng = nullptr; // Deterministic RNG
    uint64_t cycles = 0; // Profiling
    std::string error;   // Error state
};

// 5. The Virtual Machine
class GVM {
public:
    GVM();
    
    // External Function Interface Type
    // Args are popped from stack, Result is pushed
    using ExternalFunction = std::function<GVMInt(const std::vector<GVMInt>& args)>;

    // External Function Signature
    struct EFISignature {
        int arg_count;
        bool is_pure; // Side-effect free?
        // std::string return_type; // Always GVMInt for now
    };

    void RegisterFunction(uint8_t id, ExternalFunction func, EFISignature sig);
    
    // Load and Validate Bytecode (Static Analysis + HMAC)
    // If secret is provided, the last 32 bytes of bytecode are treated as HMAC-SHA256 signature
    bool Load(const std::vector<uint8_t>& bytecode, const std::string& secret = "");
    
    // Execute Loaded Code
    bool Execute(VMContext& ctx);

    // Helpers
    static std::vector<uint8_t> Serialize(const std::vector<uint8_t>& code);

private:
    std::vector<uint8_t> m_bytecode;
    
    struct ExternalFuncData {
        ExternalFunction func;
        EFISignature signature;
    };
    std::array<ExternalFuncData, 256> m_external_funcs{};
    
    // Validation Helpers
    bool ValidateJumpTargets();
};

} // namespace Core
