#include "Core/GVM.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include "Core/Crypto.h"

namespace Core {

// Instruction Cost Table (Gas)
static const uint8_t OPCODE_COST[256] = {
    0, // HALT
    1, // NOP
    // ... defaults to 1 usually, but some are expensive
};

// Helper: Get Cost
uint8_t GetCost(uint8_t op) {
    if (op == static_cast<uint8_t>(Opcode::EXT_CALL)) return 10;
    if (op == static_cast<uint8_t>(Opcode::MUL) || op == static_cast<uint8_t>(Opcode::DIV)) return 3;
    return 1;
}

GVM::GVM() {
    // Zero out external funcs
    for (auto& f : m_external_funcs) {
        f.signature.arg_count = -1; // Unused
    }
}

void GVM::RegisterFunction(uint8_t id, ExternalFunction func, EFISignature sig) {
    m_external_funcs[id] = {func, sig};
}


// ...

bool GVM::Load(const std::vector<uint8_t>& bytecode, const std::string& secret) {
    // 1. Basic Size Check
    if (bytecode.empty()) return false;
    
    // 2. HMAC Validation (If secret provided)
    if (!secret.empty()) {
        if (bytecode.size() < 32) { // Minimum size for HMAC
             return false;
        }
        
        size_t sig_len = 32; 
        size_t content_len = bytecode.size() - sig_len;
        
        std::vector<uint8_t> content(bytecode.begin(), bytecode.begin() + content_len);
        std::vector<uint8_t> provided_sig(bytecode.begin() + content_len, bytecode.end());
        
        // Calculate expected HMAC (Returns Hex String)
        std::string calculated_sig_hex = Crypto::HMAC_SHA256(secret, content);
        
        // Convert Hex to Bytes
        std::vector<uint8_t> calculated_sig_bytes;
        for (size_t i = 0; i < calculated_sig_hex.length(); i += 2) {
            std::string byteString = calculated_sig_hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
            calculated_sig_bytes.push_back(byte);
        }
        
        if (provided_sig != calculated_sig_bytes) {
            // Signature Mismatch
            return false; 
        }
        
        m_bytecode = content; // Only load actual code
    } else {
        m_bytecode = bytecode;
    }

    // 3. Validate Jumps (CFG Analysis)
    return ValidateJumpTargets();
}

bool GVM::ValidateJumpTargets() {
    size_t pc = 0;
    std::vector<bool> valid_targets(m_bytecode.size() + 1, false);
    
    // Pass 1: Mark Instruction Boundaries
    while (pc < m_bytecode.size()) {
        valid_targets[pc] = true;
        uint8_t op = m_bytecode[pc];
        pc++;
        
        if (op == static_cast<uint8_t>(Opcode::PUSH)) {
            pc += 8; // Skip operand
        } else if (op == static_cast<uint8_t>(Opcode::JMP) || 
                   op == static_cast<uint8_t>(Opcode::JZ) || 
                   op == static_cast<uint8_t>(Opcode::JNZ)) {
            pc += 4; // Skip relative offset (int32)
        } else if (op == static_cast<uint8_t>(Opcode::EXT_CALL) || 
                   op == static_cast<uint8_t>(Opcode::LOAD) || 
                   op == static_cast<uint8_t>(Opcode::STORE)) {
            pc += 1; // Skip ID/Index
        }
    }
    
    // Pass 2: Verify Jump Targets
    pc = 0;
    while (pc < m_bytecode.size()) {
        uint8_t op = m_bytecode[pc];
        uint32_t instr_start = static_cast<uint32_t>(pc);
        pc++;
        
        if (op == static_cast<uint8_t>(Opcode::PUSH)) {
            pc += 8;
        } else if (op == static_cast<uint8_t>(Opcode::JMP) || 
                   op == static_cast<uint8_t>(Opcode::JZ) || 
                   op == static_cast<uint8_t>(Opcode::JNZ)) {
            
            if (pc + 4 > m_bytecode.size()) return false;
            
            int32_t offset = 0;
            std::memcpy(&offset, &m_bytecode[pc], 4);
            pc += 4;
            
            int32_t target = instr_start + offset;
            
            // Boundary Check
            if (target < 0 || target >= static_cast<int32_t>(m_bytecode.size())) {
                return false; // Jump out of bounds
            }
            
            // Alignment Check
            if (!valid_targets[target]) {
                return false; // Jump into middle of instruction
            }
            
        } else if (op == static_cast<uint8_t>(Opcode::EXT_CALL) || 
                   op == static_cast<uint8_t>(Opcode::LOAD) || 
                   op == static_cast<uint8_t>(Opcode::STORE)) {
            pc += 1;
        }
    }
    
    return true;
}

bool GVM::Execute(VMContext& ctx) {
    const uint8_t* ip = m_bytecode.data();
    const size_t code_size = m_bytecode.size();
    
    // Zero Cycle Count
    ctx.cycles = 0;
    int gas = MAX_GAS;

    while (ctx.pc < code_size && gas > 0) {
        uint8_t op = ip[ctx.pc];
        // std::cout << "Exec Op: " << std::hex << static_cast<int>(op) << " at PC: " << std::dec << ctx.pc << std::endl;
        gas -= GetCost(op);
        ctx.cycles++;
        
        ctx.pc++; // Advance past Opcode

        switch (static_cast<Opcode>(op)) {
            case Opcode::HALT:
                return true;

            case Opcode::NOP:
                break;

            case Opcode::PUSH: {
                if (ctx.sp >= MAX_STACK) { ctx.error = "Stack Overflow"; return false; }
                if (ctx.pc + 8 > code_size) { ctx.error = "Unexpected EOF"; return false; }
                
                GVMInt val;
                std::memcpy(&val, &ip[ctx.pc], 8);
                ctx.stack[ctx.sp++] = val;
                ctx.pc += 8;
                break;
            }

            case Opcode::POP: {
                if (ctx.sp == 0) { ctx.error = "Stack Underflow"; return false; }
                ctx.sp--;
                break;
            }
            
            case Opcode::DUP: {
                if (ctx.sp == 0) { ctx.error = "Stack Underflow"; return false; }
                if (ctx.sp >= MAX_STACK) { ctx.error = "Stack Overflow"; return false; }
                ctx.stack[ctx.sp] = ctx.stack[ctx.sp - 1]; // Copy top
                ctx.sp++;
                break;
            }

            case Opcode::ADD: {
                if (ctx.sp < 2) { ctx.error = "Stack Underflow"; return false; }
                GVMInt b = ctx.stack[--ctx.sp];
                GVMInt a = ctx.stack[--ctx.sp];
                // Signed Wrap-Around (defined behavior using unsigned cast)
                ctx.stack[ctx.sp++] = static_cast<GVMInt>(static_cast<uint64_t>(a) + static_cast<uint64_t>(b));
                break;
            }
            
            case Opcode::SUB: {
                if (ctx.sp < 2) { ctx.error = "Stack Underflow"; return false; }
                GVMInt b = ctx.stack[--ctx.sp];
                GVMInt a = ctx.stack[--ctx.sp];
                ctx.stack[ctx.sp++] = static_cast<GVMInt>(static_cast<uint64_t>(a) - static_cast<uint64_t>(b));
                break;
            }
            
            case Opcode::MUL: {
                if (ctx.sp < 2) { ctx.error = "Stack Underflow"; return false; }
                GVMInt b = ctx.stack[--ctx.sp];
                GVMInt a = ctx.stack[--ctx.sp];
                ctx.stack[ctx.sp++] = static_cast<GVMInt>(static_cast<uint64_t>(a) * static_cast<uint64_t>(b));
                break;
            }
            
            case Opcode::DIV: {
                if (ctx.sp < 2) { ctx.error = "Stack Underflow"; return false; }
                GVMInt b = ctx.stack[--ctx.sp];
                GVMInt a = ctx.stack[--ctx.sp];
                if (b == 0) { ctx.error = "Divide by Zero"; return false; }
                ctx.stack[ctx.sp++] = a / b;
                break;
            }
            
            case Opcode::EQ: {
                if (ctx.sp < 2) { ctx.error = "Stack Underflow"; return false; }
                GVMInt b = ctx.stack[--ctx.sp];
                GVMInt a = ctx.stack[--ctx.sp];
                ctx.stack[ctx.sp++] = (a == b) ? 1 : 0;
                break;
            }

            case Opcode::NEQ: {
                if (ctx.sp < 2) { ctx.error = "Stack Underflow"; return false; }
                GVMInt b = ctx.stack[--ctx.sp];
                GVMInt a = ctx.stack[--ctx.sp];
                ctx.stack[ctx.sp++] = (a != b) ? 1 : 0;
                break;
            }

            case Opcode::LT: {
                if (ctx.sp < 2) { ctx.error = "Stack Underflow"; return false; }
                GVMInt b = ctx.stack[--ctx.sp];
                GVMInt a = ctx.stack[--ctx.sp];
                ctx.stack[ctx.sp++] = (a < b) ? 1 : 0;
                break;
            }

            case Opcode::GT: {
                if (ctx.sp < 2) { ctx.error = "Stack Underflow"; return false; }
                GVMInt b = ctx.stack[--ctx.sp];
                GVMInt a = ctx.stack[--ctx.sp];
                ctx.stack[ctx.sp++] = (a > b) ? 1 : 0;
                break;
            }

            case Opcode::LE: {
                if (ctx.sp < 2) { ctx.error = "Stack Underflow"; return false; }
                GVMInt b = ctx.stack[--ctx.sp];
                GVMInt a = ctx.stack[--ctx.sp];
                ctx.stack[ctx.sp++] = (a <= b) ? 1 : 0;
                break;
            }

            case Opcode::GE: {
                if (ctx.sp < 2) { ctx.error = "Stack Underflow"; return false; }
                GVMInt b = ctx.stack[--ctx.sp];
                GVMInt a = ctx.stack[--ctx.sp];
                ctx.stack[ctx.sp++] = (a >= b) ? 1 : 0;
                break;
            }
            
            case Opcode::JMP: {
                 if (ctx.pc + 4 > code_size) { ctx.error = "Unexpected EOF"; return false; }
                 int32_t offset;
                 std::memcpy(&offset, &ip[ctx.pc], 4);
                 // Target logic is relative to instruction start (opcode index)
                 // PC is currently at Operand. Start was PC-1.
                 // Target = (PC-1) + Offset.
                 // Since we validated targets in Load(), we can trust logic if implementation matches.
                 // Let's stick to standard relative jump:
                 // Update PC.
                 uint32_t instr_start = ctx.pc - 1;
                 ctx.pc = instr_start + static_cast<uint32_t>(offset);
                 break;
            }
            
            case Opcode::JZ: {
                 if (ctx.pc + 4 > code_size) { ctx.error = "Unexpected EOF"; return false; }
                 if (ctx.sp == 0) { ctx.error = "Stack Underflow"; return false; }
                 int32_t offset;
                 std::memcpy(&offset, &ip[ctx.pc], 4);
                 
                 GVMInt val = ctx.stack[--ctx.sp];
                 
                 if (val == 0) {
                     uint32_t instr_start = ctx.pc - 1;
                     ctx.pc = instr_start + static_cast<uint32_t>(offset);
                 } else {
                     ctx.pc += 4; // Skip operand
                 }
                 break;
            }
            
            case Opcode::STORE: {
                if (ctx.pc + 1 > code_size) { ctx.error = "Unexpected EOF"; return false; }
                uint8_t idx = ip[ctx.pc++];
                if (idx >= MAX_GLOBALS) { ctx.error = "Global Invalid Access"; return false; }
                if (ctx.sp == 0) { ctx.error = "Stack Underflow"; return false; }
                ctx.globals[idx] = ctx.stack[--ctx.sp];
                break;
            }
            
            case Opcode::LOAD: {
                if (ctx.pc + 1 > code_size) { ctx.error = "Unexpected EOF"; return false; }
                uint8_t idx = ip[ctx.pc++];
                if (idx >= MAX_GLOBALS) { ctx.error = "Global Invalid Access"; return false; }
                if (ctx.sp >= MAX_STACK) { ctx.error = "Stack Overflow"; return false; }
                ctx.stack[ctx.sp++] = ctx.globals[idx];
                break;
            }

            case Opcode::EXT_CALL: {
                if (ctx.pc + 1 > code_size) { ctx.error = "Unexpected EOF"; return false; }
                uint8_t func_id = ip[ctx.pc++];
                
                if (func_id >= m_external_funcs.size() || m_external_funcs[func_id].signature.arg_count == -1) {
                    ctx.error = "Invalid External Function Call"; return false;
                }
                
                int args_needed = m_external_funcs[func_id].signature.arg_count;
                if (ctx.sp < static_cast<uint32_t>(args_needed)) {
                    ctx.error = "ExtCall Stack Underflow"; return false;
                }
                
                // Collect args (reverse order, top of stack is last arg)
                std::vector<GVMInt> args;
                for (int i = 0; i < args_needed; ++i) {
                     args.push_back(ctx.stack[--ctx.sp]);
                }
                // In stack machine, arguments are usually pushed: Arg1, Arg2.
                // Stack: [Arg1, Arg2]. Pop -> Arg2. Pop -> Arg1.
                // Vector should prob be [Arg1, Arg2].
                // So reverse the pop order or reverse vector.
                // Let's reverse the vector to match signature order.
                // std::reverse(args.begin(), args.end());
                // Actually, if we pop Arg2 then Arg1, we get {Arg2, Arg1}.
                // We want {Arg1, Arg2}. So we push_back.
                // Wait.
                // Stack: Bottom [A, B] Top.
                // Pop -> B. Pop -> A.
                // Vector: {B, A}.
                // Function(A, B).
                // So we need to reverse.
                // Or just iterate backwards? No, strict pop.
                 std::vector<GVMInt> ordered_args(args_needed);
                 for (int i = 0; i < args_needed; ++i) {
                     ordered_args[args_needed - 1 - i] = args[i];
                 }
                
                GVMInt result = m_external_funcs[func_id].func(ordered_args);
                
                if (ctx.sp >= MAX_STACK) { ctx.error = "Stack Overflow on Return"; return false; }
                ctx.stack[ctx.sp++] = result;
                break;
            }
            
            case Opcode::PRINT: {
                if (ctx.sp == 0) { ctx.error = "Stack Underflow"; return false; }
                GVMInt val = ctx.stack[--ctx.sp];
                std::cout << "[GVM] Print: " << val << std::endl;
                break;
            }

            default:
                ctx.error = "Invalid Opcode";
                return false;
        }
    }
    
    if (gas <= 0) {
        ctx.error = "Gas Limit Exceeded";
        return false;
    }
    
    return true; // Execution finished normally (though normally ends with HALT)
}

} // namespace Core
