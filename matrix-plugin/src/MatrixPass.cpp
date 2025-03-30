#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

#include <fstream>
#include <bitset>

using namespace llvm;

// Encode 19-bit binary instruction as string
std::string encodeInstruction(uint8_t opcode, bool rd, bool wr, uint16_t row, uint8_t opExt = 0) {
    uint32_t inst = 0;
    inst |= (opcode & 0b11) << 17;             // bits 18–17: Opcode
    inst |= (opExt & 0b111111) << 11;          // bits 16–11: Core/Op code
    inst |= (rd ? 1 : 0) << 10;                // bit 10: Read
    inst |= (wr ? 1 : 0) << 9;                 // bit 9: Write
    inst |= (row & 0x1FF);                     // bits 8–0: Row address
    return std::bitset<19>(inst).to_string();
}

struct MatrixPass : PassInfoMixin<MatrixPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        errs() << "MatrixPass is running on: " << F.getName() << "\n";
        std::ofstream outFile("output.pim", std::ios::app);

        int rowCounter = 0;

        for (auto &BB : F) {
            for (auto &I : BB) {
                if (auto *op = dyn_cast<BinaryOperator>(&I)) {
                    unsigned opcode = op->getOpcode();
                    uint8_t execCode = 0;

                    if (opcode == Instruction::Add) execCode = 1;
                    else if (opcode == Instruction::Sub) execCode = 2;
                    else if (opcode == Instruction::Mul) execCode = 3;
                    else if (opcode == Instruction::SDiv) execCode = 4;
                    else continue;

                    // READ_ROW A
                    outFile << encodeInstruction(0b01, true, false, rowCounter++) << "\n";
                    // READ_ROW B
                    outFile << encodeInstruction(0b01, true, false, rowCounter++) << "\n";
                    // EXECUTE OP
                    outFile << encodeInstruction(0b10, false, false, 0, execCode) << "\n";
                    // WRITE_ROW C
                    outFile << encodeInstruction(0b01, false, true, rowCounter++) << "\n";
                }
            }
        }

        // END instruction
        outFile << encodeInstruction(0b11, false, false, 0) << "\n";
        outFile.close();
        return PreservedAnalyses::all();
    }
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "MatrixPass", "v1.0",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "matrixpass") {
                        FunctionPassManager FPM;
                        FPM.addPass(MatrixPass());
                        MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));
                        return true;
                    }
                    return false;
                });
        }
    };
}

