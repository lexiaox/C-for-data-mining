# ==============================================================================
# 项目配置 (Project Configuration)
# ==============================================================================
cmake_minimum_required(VERSION 3.10)
project(SLE_Aging_Analysis C)

# 设置 C 标准为 C99 或 C11 (确保跨平台兼容性)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 告诉 CMake 头文件 (.h) 在哪里
include_directories(src)

# ==============================================================================
# 构建目标 (Build Targets)
# ==============================================================================

# --- Step 1: 样本分组 ---
# 依赖: step1_grouping.c 和 common.c
add_executable(step1 src/step1_grouping.c src/common.c)

# --- Step 2: 核心分析 (T-test) ---
# 依赖: step2_analysis.c 和 common.c
add_executable(step2 src/step2_analysis.c src/common.c)

# 链接数学库 (Math Library)
# Linux/Unix 下需要显式链接 libm (-lm)，Windows (MSVC) 通常自动包含
if(UNIX)
    target_link_libraries(step2 m)
endif()

# --- Step 3: 后处理 (排序与筛选) ---
# 依赖: step3_post_process.c
# 注意：如果 step3 也用了 common.c 里的函数，请把 src/common.c 加进去
add_executable(step3 src/step3_post_process.c)

# Step 3 也用到了 fabs (数学函数)，为了保险起见链接数学库
if(UNIX)
    target_link_libraries(step3 m)
endif()

