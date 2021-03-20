/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

template<class K, class V, class C>
ir::Map<K, V, C>::Map() noexcept
{}

template<class K, class V, class C>
ir::Map<K, V, C>::Map(const QuietMap<K, V, C> &map) noexcept : QuietMap<K, V, C>::QuietMap(map)
{}