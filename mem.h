//-------------------------------------------------------------------
// 数値地図ファイル関係の処理
// 拡張子が".mem"なので、モジュール名をmemとする。
//-------------------------------------------------------------------
#include "types.h"
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/strided.hpp>
namespace filesystem = boost::filesystem;
namespace adaptors = boost::adaptors;

//-------------------------------------------------------------------
// 座標索引について
//  iLower    : 領域area (.MEMファイル内)におけるpointの座標
//  iHigher   : 領域areaの座標
//  iLocation : 全体におけるpointの座標、ゼロ点補正前
//  iPlace    : 全体におけるpointの座標、ゼロ点補正後
//-------------------------------------------------------------------
#pragma  warning (disable :4351)
namespace mem
{
	//-------------------------------------------------------------------
	// 定数
	//-------------------------------------------------------------------

	static int const LEN = 200;// メッシュ１辺の長さ
//	static int const SEA = -999;// 海扱いの標高

	//-------------------------------------------------------------------
	// Mem(2次メッシュ)タグ型
	//-------------------------------------------------------------------
	class Tag
	{
		int index_;
	public:
		// 生成
		Tag();
		Tag(int index);
		Tag(filesystem::path const& path);
		static Tag fromFile(filesystem::path const& path){ return Tag(path); }
		// 要素
		int index()const{ return index_; }
		filesystem::path path()const;
		types::IYX iHigher() const;// { return{ iY(), iX() }; }
	};
	std::vector<Tag>getAreaTags(std::vector<std::string> const& areaFiles);
		// 2次メッシュ領域座標範囲取得
	//  出力: ((iYa最小,iXa最小),(iYa最大,iXa最大) )
	std::pair<types::IYX, types::IYX  >	extentHigher(std::vector<Tag> const& tags);

	// 50m×50m領域座標範囲取得(マージン付)
	//  出力: ((iYp最小,iXp最小),(iYp最大,iXp最大) )
	std::pair<types::IYX, types::IYX  >	extentLocation(
		filesystem::path const& basePath,
		std::vector<Tag> const& tags);
	// 比較
	inline bool operator==(Tag const& lhs, Tag const& rhs){ return lhs.index() == rhs.index(); }
	inline bool operator!=(Tag const& lhs, Tag const& rhs){ return lhs.index() != rhs.index(); }

	//-------------------------------------------------------------------
	// Mem(2次メッシュ)メモリ型
	//-------------------------------------------------------------------
	// ※ iPosは、プログラム内では単に"i"として用いる。
	//    あるいは、iLowerを取得するためのindexという感覚で、
	//    iiLowerとしたりする。
	class Area
	{
		Tag tag_;
		short data_[LEN*LEN];
	public:
		static types::IYX iLower(int iiLower)
		{
			return{ iiLower / LEN, iiLower % LEN };
		}
		static int iiLower(types::IYX const& iLower)
		{
			return iLower.iY*LEN + iLower.iX;
		}
		// 生成
		Area(){}
		// Area() = delete;

		explicit Area(filesystem::path const& path);

		// 操作
		void read(filesystem::path const& path);
		// 要素
		//
		Tag tag()const{ return tag_; }
		//(標高データ)
		short operator()(int iiLower)const
		{
			return data_[iiLower];
		}
		short operator()(types::IYX const& iLower)const
		{
			return data_[iiLower(iLower)];
		}

		// 横方向の線
		boost::iterator_range<short const*> lineY(int iY)const
		{ 
			return boost::make_iterator_range_n(&data_[iY*LEN], LEN);
		}
		// 縦方向の線
		boost::range_detail::strided_range<const boost::iterator_range<short const*> >lineX(int iX)const
		{
			return boost::make_iterator_range_n(&data_[iX], LEN*LEN) | adaptors::strided(LEN);
		}
	};
	//-------------------------------------------------------------------
	//  地図ファイルパスの一覧を取得
	//    (1)現在のフォルダの近くにある"dmap"フォルダの下を検索
	//    (2)\数字4桁\数字6桁.MEM"となるファイルを取得する。
	//    (3) 数字4桁は数字6桁の上位4桁と一致する。
	//-------------------------------------------------------------------
	std::vector<std::string> areaFiles();
	//-------------------------------------------------------------------
	//  地図ファイルパスの基礎パスを取得
	//   ⇒基礎が不明確な場合は、警告を投げる
	//-------------------------------------------------------------------
	filesystem::path basePath(std::vector<std::string> const& areaFiles);
	//-------------------------------------------------------------------
	// 座標合成
	//   ※ iPlaceの前、(原点補正されてない)
	//-------------------------------------------------------------------
	int iLocation(int iUpper, int iLower);
	types::IYX iLocation(types::IYX iUpper, types::IYX iLower);


}
