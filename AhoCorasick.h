#ifndef __AHOCORASICK_H__
#define __AHOCORASICK_H__

#include <string>
#include <memory>
#include <cstring>


template <char DictFirstChar, char DictLastChar>
class AhoCorasick
{
public:
	AhoCorasick();
	~AhoCorasick();

	void add_pattern( std::string const& pattern );
	template <typename Func> void find_patterns( std::string const& str, Func visitor );

private:
	static constexpr int DictionarySize = DictLastChar - DictFirstChar + 1;

	struct TrieNode
	{
		TrieNode( TrieNode* parent, int path ) : pattern(), parent_path( path ), parent( parent ), move(), suffix(), leaf_suffix() { std::memset( move, 0, sizeof( move ) ); }

		std::string               pattern;
		int                       parent_path;
		TrieNode*                 parent;
		std::unique_ptr<TrieNode> children[DictionarySize];
		TrieNode*                 move[DictionarySize];
		TrieNode*                 suffix;
		TrieNode*                 leaf_suffix;
	};

	TrieNode* get_move( TrieNode* node, int path );
	TrieNode* get_suffix( TrieNode* node );
	TrieNode* get_leaf_suffix( TrieNode* node );

	TrieNode root_;
};


template <char DictFirstChar, char DictLastChar>
AhoCorasick<DictFirstChar, DictLastChar>::AhoCorasick()
	: root_( &root_, 0 )
{
}


template <char DictFirstChar, char DictLastChar>
AhoCorasick<DictFirstChar, DictLastChar>::~AhoCorasick()
{
}


template <char DictFirstChar, char DictLastChar>
void AhoCorasick<DictFirstChar, DictLastChar>::add_pattern( std::string const& pattern )
{
	auto node = &root_;
	for (auto c : pattern)
	{
		int const path = c - DictFirstChar;
		if (!node->children[path])
			node->children[path].reset( new TrieNode( node, path ) );
		node = node->children[path].get();
	}
	node->pattern = pattern;
}


template <char DictFirstChar, char DictLastChar>
typename AhoCorasick<DictFirstChar, DictLastChar>::TrieNode* AhoCorasick<DictFirstChar, DictLastChar>::get_move( TrieNode* node, int path )
{
	if (!node->move[path])
	{
		if (!node->children[path])
			node->move[path] = node == &root_ ? node : get_move( get_suffix( node ), path );
		else
			node->move[path] = node->children[path].get();
	}
	return node->move[path];
}


template <char DictFirstChar, char DictLastChar>
typename AhoCorasick<DictFirstChar, DictLastChar>::TrieNode* AhoCorasick<DictFirstChar, DictLastChar>::get_suffix( TrieNode* node )
{
	if (!node->suffix)
	{
		if (node->parent == &root_)
			node->suffix = node->parent;
		else
			node->suffix = get_move( get_suffix( node->parent ), node->parent_path );
	}
	return node->suffix;
}


template <char DictFirstChar, char DictLastChar>
typename AhoCorasick<DictFirstChar, DictLastChar>::TrieNode* AhoCorasick<DictFirstChar, DictLastChar>::get_leaf_suffix( TrieNode* node )
{
	if (!node->leaf_suffix)
	{
		auto suffix = get_suffix( node );
		if (suffix == &root_ || !suffix->pattern.empty())
			node->leaf_suffix = suffix;
		else
			node->leaf_suffix = get_leaf_suffix( suffix );
	}
	return node->leaf_suffix;
}


template <char DictFirstChar, char DictLastChar> template <typename Func>
void AhoCorasick<DictFirstChar, DictLastChar>::find_patterns( std::string const& str, Func visitor )
{
	auto   node = &root_;
	size_t pos  = 0;
	for (char c : str)
	{
		++pos;
		int const path = c - DictFirstChar;
		node = get_move( node, path );
		for (auto n = node; n != &root_; n = get_leaf_suffix( n ))
			if (!n->pattern.empty())
				visitor( n->pattern, pos - n->pattern.length() );
	}
}

#endif // __AHOCORASICK_H__
