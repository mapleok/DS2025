import matplotlib.pyplot as plt
import numpy as np

# 设置中文字体（避免中文乱码）
plt.rcParams['font.sans-serif'] = ['SimHei']  # 黑体
plt.rcParams['axes.unicode_minus'] = False

# 实验数据（从运行结果中提取）
sizes = [100, 500, 1000, 2000, 5000, 10000]

# 1. 单独排序性能（随机分布）
random_sort = {
    '冒泡排序': [69, 1388, 5845, 22699, 147384, 620143],
    '选择排序': [49, 1304, 3983, 13965, 90738, 358083],
    '二分插入排序': [21, 463, 1847, 6783, 45269, 174088],
    '快速排序': [6, 44, 99, 247, 728, 1597]
}

# 2. 单独排序性能（聚集分布）
clustered_sort = {
    '冒泡排序': [71, 1341, 5447, 22901, 141218, 603410],
    '选择排序': [111, 2196, 5453, 16860, 89809, 361105],
    '二分插入排序': [54, 445, 1856, 6948, 42474, 176552],
    '快速排序': [6, 41, 104, 246, 697, 1562]
}

# 3. 排序+NMS性能（随机分布）
random_nms = {
    '冒泡+NMS': [341, 7616, 29264, 110572, 659031, 2518888],
    '选择+NMS': [284, 7217, 26368, 99707, 605201, 2292894],
    '二分插入+NMS': [248, 6997, 25339, 93498, 560184, 2078920],
    '快速+NMS': [241, 5999, 23395, 87178, 521301, 1886647]
}

# 4. 排序+NMS性能（聚集分布）
clustered_nms = {
    '冒泡+NMS': [329, 6591, 25897, 100833, 547712, 2115286],
    '选择+NMS': [264, 6321, 23576, 84754, 487091, 1860165],
    '二分插入+NMS': [246, 5926, 22414, 81622, 446547, 1683056],
    '快速+NMS': [234, 5192, 20664, 72900, 395710, 1499271]
}

# 定义颜色和线型（区分不同算法）
colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4']
linestyles = ['-', '--', '-.', ':']
markers = ['o', 's', '^', 'D']

# ========== 图1：不同排序算法性能对比（随机分布） ==========
plt.figure(figsize=(10, 6))
for i, (alg, data) in enumerate(random_sort.items()):
    plt.plot(sizes, data, color=colors[i], linestyle=linestyles[i], marker=markers[i], 
             markersize=6, label=alg, linewidth=2)
plt.xscale('log')  # 横坐标对数刻度（更清晰展示规模差异）
plt.yscale('log')  # 纵坐标对数刻度（适配数值范围差异）
plt.xlabel('数据规模（边界框数量）', fontsize=12)
plt.ylabel('运行时间（微秒）', fontsize=12)
plt.title('随机分布下不同排序算法性能对比', fontsize=14, fontweight='bold')
plt.legend(fontsize=10)
plt.grid(True, alpha=0.3)
plt.savefig('随机分布_排序算法性能.png', dpi=300, bbox_inches='tight')
plt.close()

# ========== 图2：不同排序算法性能对比（聚集分布） ==========
plt.figure(figsize=(10, 6))
for i, (alg, data) in enumerate(clustered_sort.items()):
    plt.plot(sizes, data, color=colors[i], linestyle=linestyles[i], marker=markers[i], 
             markersize=6, label=alg, linewidth=2)
plt.xscale('log')
plt.yscale('log')
plt.xlabel('数据规模（边界框数量）', fontsize=12)
plt.ylabel('运行时间（微秒）', fontsize=12)
plt.title('聚集分布下不同排序算法性能对比', fontsize=14, fontweight='bold')
plt.legend(fontsize=10)
plt.grid(True, alpha=0.3)
plt.savefig('聚集分布_排序算法性能.png', dpi=300, bbox_inches='tight')
plt.close()

# ========== 图3：排序+NMS性能对比（两种分布合并） ==========
plt.figure(figsize=(12, 7))
# 随机分布
for i, (alg, data) in enumerate(random_nms.items()):
    plt.plot(sizes, data, color=colors[i], linestyle=linestyles[i], marker=markers[i], 
             markersize=5, label=f'{alg}（随机）', linewidth=2, alpha=0.8)
# 聚集分布
for i, (alg, data) in enumerate(clustered_nms.items()):
    base_alg = alg.split('+')[0]
    plt.plot(sizes, data, color=colors[i], linestyle=linestyles[i], marker=markers[i], 
             markersize=5, label=f'{alg}（聚集）', linewidth=2, alpha=0.5, markerfacecolor='none')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('数据规模（边界框数量）', fontsize=12)
plt.ylabel('运行时间（微秒）', fontsize=12)
plt.title('排序+NMS 算法性能对比（两种分布）', fontsize=14, fontweight='bold')
plt.legend(fontsize=9, loc='upper left', bbox_to_anchor=(1, 1))
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('排序+NMS_性能对比.png', dpi=300, bbox_inches='tight')
plt.close()

# ========== 图4：数据分布对性能的影响（10000个框时） ==========
plt.figure(figsize=(10, 6))
# 提取10000个框的数据（索引5）
alg_names = ['冒泡', '选择', '二分插入', '快速']
sort_random_10000 = [random_sort[alg][5] for alg in random_sort.keys()]
sort_clustered_10000 = [clustered_sort[alg][5] for alg in clustered_sort.keys()]
nms_random_10000 = [random_nms[f'{alg}+NMS'][5] for alg in alg_names]
nms_clustered_10000 = [clustered_nms[f'{alg}+NMS'][5] for alg in alg_names]

x = np.arange(len(alg_names))
width = 0.2  # 柱状图宽度

# 绘制柱状图
plt.bar(x - 1.5*width, sort_random_10000, width, label='排序（随机）', color='#FF6B6B', alpha=0.8)
plt.bar(x - 0.5*width, sort_clustered_10000, width, label='排序（聚集）', color='#FF8E53', alpha=0.8)
plt.bar(x + 0.5*width, nms_random_10000, width, label='排序+NMS（随机）', color='#4ECDC4', alpha=0.8)
plt.bar(x + 1.5*width, nms_clustered_10000, width, label='排序+NMS（聚集）', color='#45B7D1', alpha=0.8)

plt.xlabel('算法类型', fontsize=12)
plt.ylabel('运行时间（微秒）', fontsize=12)
plt.title('数据分布对性能的影响（10000个边界框）', fontsize=14, fontweight='bold')
plt.xticks(x, alg_names)
plt.legend(fontsize=10)
plt.yscale('log')  # 对数刻度，适配数值差异
plt.grid(True, alpha=0.3, axis='y')
plt.savefig('数据分布_性能影响.png', dpi=300, bbox_inches='tight')
plt.close()

print("可视化图表生成完成！共4张：")
print("1. 随机分布_排序算法性能.png")
print("2. 聚集分布_排序算法性能.png")
print("3. 排序+NMS_性能对比.png")
print("4. 数据分布_性能影响.png")