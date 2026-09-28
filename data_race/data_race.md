# std::adopt_lock 与 std::defer_lock
- std::adpot_lock 已经获取锁；接管现有的锁，负责解锁
- std::defer_lock 尚未获取锁；先创建对象，稍后再锁
# Atomic
- load 原子地读取当前值
- store(v) 原子地写入一个值
- fetch_add(v) 原子地加上 v ，并返回修改前的值
## Compare And Exchange(CAS)
- 值没变，才写入
- 解决多次判断（load+store）没办法原子的问题
- 相关方法
	- compare_exchange_weak
	- compare_exchang_strong
	- weak 允许偶发的伪失败
- 准备在循环里重试：用 compare_exchange_weak
- 尝试一次，并且需要准确知道这一次是否因值不匹配而失败：用 compare_exchange_strong