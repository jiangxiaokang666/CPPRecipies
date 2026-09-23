# 负载因子
- load_factor = size() / bucket_count()
- 代表平均每个桶装多少元素
- max_load_factor() 是容器控制扩容的阈值。插入元素使桶逐渐拥挤时，容器可能增加桶数。
# find
- 查找复杂度，平均 O(1)，最坏 O(N)
# erase
- 复杂度，平局 O(1)，最坏 O(N)
- 返回值在这里是 0 或 1：0 表示没找到，1 表示删除成功。
# rehash
- void rehash( size_type count )
- count 桶的个数，重新安排元素所属的桶
	- 迭代器失效
	- 元素的引用还在
# reserve
- void reserve( size_type count );
- count 元素容量，减少插入过程中的扩容

# KeyEqual 和 KeyHash
- 如果 KeyEqual(a, b) 为 true，那么 Hash(a) 必须等于 Hash(b)。
- 反过来不要求成立