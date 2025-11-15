// swift-tools-version:5.9
import PackageDescription

let package = Package(
    name: "UniLang",
    platforms: [
        .iOS(.v14)
    ],
    products: [
        .library(
            name: "UniLangCore",
            targets: ["UniLangCore"]
        )
    ],
    targets: [
        .target(
            name: "UniLangCore",
            dependencies: [],
            path: "Sources"
        )
    ]
)
