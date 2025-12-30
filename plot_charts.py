import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np
import os

# ================= 配置区 =================
# 确保这个路径是对的
csv_file_path = 'data/GSE277909_genecounts_SLE_bulk.csv' 

# 字体设置 (Ubuntu)
font_path = '/usr/share/fonts/truetype/wqy/wqy-zenhei.ttc'
if os.path.exists(font_path):
    fm.fontManager.addfont(font_path)
    plt.rcParams['font.family'] = 'WenQuanYi Zen Hei'
else:
    plt.rcParams['font.sans-serif'] = ['SimHei', 'DejaVu Sans']

plt.style.use('dark_background')

# ================= 1. 读取真实数据 =================
print("正在读取 CSV 文件 (可能需要几秒钟)...")

# 使用 pandas 读取
# index_col=0 表示第一列是基因名，作为索引
try:
    df = pd.read_csv(csv_file_path, index_col=0)
except FileNotFoundError:
    print(f"❌ 错误：找不到文件 {csv_file_path}")
    print("请确认你的 CSV 文件确实在这个路径下。")
    exit(1)

# 🚨 关键步骤：清洗数据 (修复了 KeyError 问题) 🚨

# 获取最后一行数据
last_row = df.iloc[-1]
# 计算除去最后一行之外的所有行之和 (所有样本)
sum_without_last = df.iloc[:-1].sum()

# 验证逻辑：只拿第一列（第一个样本）来做验证
# 【修复点】：使用 .iloc[0] 明确按位置取第一个值，避免 KeyError
val_last = df.iloc[-1].iloc[0]          # 最后一行的第一个值
val_sum = df.iloc[:-1].iloc[:, 0].sum() # 第一列除去最后一行后的总和

print(f"检查第一个样本:")
print(f"  - 最后一行数值: {val_last}")
print(f"  - 前面所有行之和: {val_sum}")

real_library_sizes = []

# 判断：如果最后一行大概等于前面的总和 (允许 100 的误差)
if abs(val_last - val_sum) < 100:
    print(">>> 确认：最后一行是汇总行 (Total)，将其剔除 <<<")
    # 使用 sum_without_last 作为真实的 Library Sizes
    real_library_sizes = sum_without_last.values
else:
    print(">>> 警告：最后一行看起来像普通基因，保留全量计算 <<<")
    # 如果不像汇总行，就直接算全列和
    real_library_sizes = df.sum().values

# ================= 2. 画真实的柱状图 =================
# 排序
real_library_sizes.sort()
samples = np.arange(len(real_library_sizes))

# 单位换算成 Million (M)
sizes_in_M = real_library_sizes / 1000000.0
max_val = sizes_in_M.max()
min_val = sizes_in_M.min()
gap = max_val / min_val

fig, ax = plt.subplots(figsize=(10, 6))
bars = ax.bar(samples, sizes_in_M, color='#1E90FF', alpha=0.9, width=1.0)

# 标注
ax.annotate(f'最大值: {max_val:.1f}M Reads', 
            xy=(len(samples)-1, max_val), xytext=(len(samples)-80, max_val),
            arrowprops=dict(facecolor='red', shrink=0.05), fontsize=12, color='red')

ax.annotate(f'最小值: {min_val:.2f}M Reads', 
            xy=(0, min_val), xytext=(20, max_val/4),
            arrowprops=dict(facecolor='white', shrink=0.05), fontsize=12, color='white')

ax.text(len(samples)/2, max_val/2, f'{gap:.0f}倍 贫富差距!', 
        ha='center', fontsize=20, color='yellow', fontweight='bold')

ax.set_title('真实数据的测序深度分布 (Library Sizes)', fontsize=16, pad=20)
ax.set_ylabel('文库大小 (百万 Reads)', fontsize=12)
ax.set_xlabel(f'{len(samples)} 个样本 (按大小排序)', fontsize=12)
ax.grid(True, linestyle='--', alpha=0.3)

plt.tight_layout()
output_file = 'chart2_real_data.png'
plt.savefig(output_file, dpi=300)
print(f"✅ 真实图表已生成: {output_file}")
