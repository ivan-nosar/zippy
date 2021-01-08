const fs = require("fs");
const path = require("path");
const cp = require('child_process');

const usePredefined = process.argv.some(arg => arg === "-pd" || arg === "--predefined");
const algorithmFlagIndex = process.argv.findIndex(arg => arg === "-a" || arg === "--algorithm");
if (algorithmFlagIndex === -1 || !process.argv[algorithmFlagIndex + 1]) {
    console.error("The algorithm must be provided. For example: -a sha256");
    return 1;
}
const algorithm = process.argv[algorithmFlagIndex + 1];

const standardAlgorithmCommands = {
    "sha256": { command: "/usr/local/bin/shasum -a 256", parser: (output) => output.split("  ")[0].trim() },
    "md5": { command: "/sbin/md5", parser: (output) => output.split(" = ")[1].trim() }
}

if (usePredefined) {
    console.log("Running predefined tests");
    runPredefinedTests()
        .then(process.exit);
} else {
    console.log("Running random generated tests");
    runRandomGeneratedTests()
        .then(process.exit);
}

async function runPredefinedTests() {
    const dirPath = "testFiles";
    const dir = await fs.promises.opendir(dirPath);
    const assertions = [];
    for await (const file of dir) {
        const filePath = path.join(dirPath, file.name);
        console.log(`Testing the ${filePath}`);

        // Execute standard utility
        const standardOutput = standardAlgorithmCommands[algorithm].parser(
            cp.execSync(`${standardAlgorithmCommands[algorithm].command} ${filePath}`).toString("utf-8").split("  ")[0]
        );

        // Execute zippy
        const zippyOutput = cp.execSync(`out/zippy ${algorithm} ${filePath}`).toString("utf-8").trim();

        // Comparing the results
        if (standardOutput !== zippyOutput) {
            assertions.push({
                file: `${filePath}`,
                expectedHash: standardOutput,
                actualHash: zippyOutput
            });
        }
    }

    if (assertions.length) {
        console.error("Some tests has been failed:");
        assertions.forEach(assertion => {
            console.error(
                `\n\tFile: ${assertion.file}\n\tExpected hash: ${assertion.expectedHash}\n\tActual hash:   ${assertion.actualHash}\n`
            );
        });
        return 1;
    }

    console.log("All tests has been passed!");
    return 0;
}

async function runRandomGeneratedTests() {
    const testSuitsDirectory = "randomTestFiles";
    if (!fs.existsSync(testSuitsDirectory)) {
        fs.mkdirSync(testSuitsDirectory);
    }

    const fileConfigs = [
        {fileSize: 0, repetitions: 1},
        {fileSize: 1, repetitions: 1},
        {fileSize: 2, repetitions: 10},
        {fileSize: 10, repetitions: 10},
        {fileSize: 54, repetitions: 10},
        {fileSize: 55, repetitions: 10},
        {fileSize: 56, repetitions: 10},
        {fileSize: 60, repetitions: 10},
        {fileSize: 64, repetitions: 10},
        {fileSize: 128, repetitions: 10},
        {fileSize: 1000, repetitions: 10},
        {fileSize: 5000, repetitions: 10},
        {fileSize: 10 * 1024 * 1024, repetitions: 2},
        {fileSize: 50 * 1024 * 1024, repetitions: 2},
        {fileSize: 100 * 1024 * 1024, repetitions: 2},
        // {fileSize: 1024 * 1024 * 1024, repetitions: 1},
    ];

    const assertions = [];
    try {
        for (const config of fileConfigs) {
            console.log(`Testing of ${config.fileSize} bytes files`);

            for (let i = 0; i < config.repetitions; i++) {
                const filePath = await generateTestFile(testSuitsDirectory, config.fileSize, i);

                // Execute standard utility
                const standardOutput = standardAlgorithmCommands[algorithm].parser(
                    cp.execSync(`${standardAlgorithmCommands[algorithm].command} ${filePath}`).toString("utf-8").split("  ")[0]
                );

                // Execute zippy
                const zippyOutput = cp.execSync(`out/zippy ${algorithm} ${filePath}`).toString("utf-8").trim();

                // Comparing the results
                if (standardOutput !== zippyOutput) {
                    assertions.push({
                        fileSize: config.fileSize,
                        expectedHash: standardOutput,
                        actualHash: zippyOutput
                    });
                }

                fs.rmSync(filePath);
            }
        }
    } catch (error) {
        console.error("Some error has been occurred. Details:");
        console.log(error);
        return 1;
    } finally {
        fs.rmdirSync(testSuitsDirectory, {recursive: true});
    }

    if (assertions.length) {
        console.error("Some tests has been failed:");
        assertions.forEach(assertion => {
            console.error(
                `\n\tFile size: ${assertion.fileSize}\n\tExpected hash: ${assertion.expectedHash}\n\tActual hash:   ${assertion.actualHash}\n`
            );
        });
        return 1;
    }

    console.log("All tests has been passed!");

    return 0;
}

async function generateTestFile(fileDirectory, fileSize, repetition) {
    const filePath = path.join(fileDirectory, `file-${fileSize}-${repetition}`);

    let dataBuffer = Buffer.alloc(fileSize);
    for (let i = 0; i < fileSize; i++) {
        const randomByte = Math.floor(Math.random() * 256) - 128;
        dataBuffer.writeInt8(randomByte, i);
    }

    fs.writeFileSync(filePath, dataBuffer);
    dataBuffer = null;

    return filePath;
}
