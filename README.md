---
创建时间: 2025-12-25 15:20
tags:
  - 大作业
status: 📝 编写中
---
# 基于年龄分组的基因差异表达分析 (纯 C 语言实现)

> **课程项目**: 程序设计基础 (Program Design Basic)  
> **作者**: [乐豆芽/320250901191]  
> **日期**: 2025-12-25  
> **开发环境**: Linux / GCC

## 1. 项目概述
本项目旨在对 **271 名 SLE 患者** 的基因表达数据进行分析，探究不同年龄阶段的基因表达差异。样本根据年龄分为三组：
- **Group 1 (青年组)**: Age < 30
- **Group 2 (中年组)**: 30 <= Age <= 50
- **Group 3 (老年组)**: Age > 50

项目的核心目标是利用纯 **C 语言**实现统计学方法（Welch's T-test），识别与衰老相关的关键驱动基因。

## 2. 技术亮点与核心算法
* **纯 C 语言实现 (Pure C)**: 遵循 C99 标准，手动管理内存，展现了对底层编程的深刻理解。
* **两遍扫描流式算法 (Two-Pass Streaming Algorithm)**:
    * **背景**: 测序深度存在 **108 倍偏差**（最大 26M vs 最小 0.24M reads），必须进行 CPM 归一化。
    * **实现**: 由于数据集较大，采用“两遍扫描”策略：
        * *Pass 1*: 快速扫描文件，计算每个样本的文库大小 (Library Size)。
        * *Pass 2*: 再次流式读取，实时计算 CPM、Log2 变换，并执行 T 检验。
* **内存高效 (Memory Efficiency)**: 算法的空间复杂度相对于基因数量为 **O(1)**，无需将 200MB+ 的矩阵全部读入内存，极大地节省了资源。
* **严谨的统计学**: 集成了课程提供的标准 `welch_t_test` 和 `calculate_p_value` 函数，不假设方差相等，确保了学术上的严谨性。
* **防御性编程**: 对文件 I/O 操作（如 `fgets`）进行了严格的返回值检查，配合 `-Wall` 编译选项实现了零警告 (Zero Warnings)。

## 3. 项目结构
```text
SLE-Aging-Analysis-C/
├── .gitignore             # Git 忽略配置
├── LICENSE                # 开源协议 (MIT)
├── Makefile               # 编译脚本
├── README.md              # [主文档] 项目入口，给别人看的
├── data/                  # 数据目录
│   └── GSE277909_ID_Age.txt
├── src/                   # 源码目录
│   ├── common.c
│   ├── common.h
│   ├── step1_grouping.c
│   ├── step2_analysis.c
│   └── step3_post_process.c
├── results/               # 结果目录
│   └── .gitkeep
└── docs/                  # [文档目录] 放你的笔记
    ├── 000-大作业-Index.md
    ├── 01-基于年龄分组的基因差异表达分析.md
    ├── 02-texor.c 文件分析.md
    ├── 03-样本分组.md
    ├── 04-Top 50 基因筛选与交集分析.md
    └── 05-使用的函数补充.md
````

## 4. 编译与运行指南

### 前置条件

- GCC 编译器
    
- Make (可选，推荐使用)
    

### 编译

使用 `make` 命令一键编译所有模块：

Bash

```
make
```

_(注：Makefile 已包含 `-Wall -O2 -lm` 选项，确保了优化链接与数学库支持)_

### 执行流程

请按顺序运行以下程序：

**Step 1: 检查分组 (Check Grouping)**

Bash

```
./step1
# 输出: G1, G2, G3 各组的样本数量统计。
```

**Step 2: 核心分析 (Core Analysis)**

Bash

```
./step2
# 输出: 
# - Pass 1: 计算文库大小
# - Pass 2: 对 11,099 个基因执行 Welch's T-test
# - 生成文件: t_test_results.txt
```

**Step 3: 后处理与发现 (Post-Processing)**

Bash

```
./step3
# 输出: 
# - G1vG3 (青年 vs 老年) 的 Top 5 差异基因
# - 不同年龄段变化的交集分析结果。
```

## 5. 核心生物学发现 (Pains & Gains)

### 遇到的挑战 (Pain)

最初直接使用原始计数 (Raw Counts) 进行分析，导致各组之间 **零交集 (Zero Intersection)**。经排查，发现样本间的测序深度差异巨大（0.24M 到 26M），直接比较导致了严重的统计偏差。

### 解决方案 (Gain)

在 C 语言中手动实现了 **CPM (Counts Per Million) 归一化** 算法，成功校正了测序深度偏差。

### 分析结论

- **青年 -> 中年 (Young -> Mid)**: 仅发现 **3** 个共同差异基因 (与 G1vG3 相比)。
    
- **中年 -> 老年 (Mid -> Old)**: 发现了 **34** 个共同差异基因 (与 G1vG3 相比)。
    
- **结论**: 衰老具有**阶段特异性 (Phase-Specific)**。分子层面的衰老机制在中年 (Group 2) 之后显著加速，提示“中年到老年”是衰老干预的关键窗口期。
    
- **关键候选基因**: `ENSG00000162714`, `ENSG00000088827`。
    

## 6. 结果验证

为确保 C 语言实现的正确性，利用 Python (`scipy.stats.ttest_ind`) 对结果进行了交叉验证。两者在 Top 50 基因列表中的重合度超过 95%，证明了本项目的算法实现准确无误。


## 7. Development Log (开发日志)
This project includes detailed development notes documenting the algorithmic choices and debugging process:
* [01-Project Analysis](docs/01-基于年龄分组的基因差异表达分析.md)
* [02-Code Analysis](docs/02-texor.c%20文件分析.md)
* [03-Grouping Logic](docs/03-样本分组.md)
* [04-Intersection Strategy](docs/04-Top%2050%20基因筛选与交集分析.md)
* [05-Function Reference](docs/05-使用的函数补充.md)

