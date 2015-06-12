//-------------------------------------------------------------------
// ���l�n�}�t�@�C���֌W�̏���
// �g���q��".mem"�Ȃ̂ŁA���W���[������mem�Ƃ���B
//-------------------------------------------------------------------
#include "types.h"
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/strided.hpp>
namespace filesystem = boost::filesystem;
namespace adaptors = boost::adaptors;

//-------------------------------------------------------------------
// ���W�����ɂ���
//  iLower    : �̈�area (.MEM�t�@�C����)�ɂ�����point�̍��W
//  iHigher   : �̈�area�̍��W
//  iLocation : �S�̂ɂ�����point�̍��W�A�[���_�␳�O
//  iPlace    : �S�̂ɂ�����point�̍��W�A�[���_�␳��
//-------------------------------------------------------------------
#pragma  warning (disable :4351)
namespace mem
{
	//-------------------------------------------------------------------
	// �萔
	//-------------------------------------------------------------------

	static int const LEN = 200;// ���b�V���P�ӂ̒���
//	static int const SEA = -999;// �C�����̕W��

	//-------------------------------------------------------------------
	// Mem(2�����b�V��)�^�O�^
	//-------------------------------------------------------------------
	class Tag
	{
		int index_;
	public:
		// ����
		Tag();
		Tag(int index);
		Tag(filesystem::path const& path);
		static Tag fromFile(filesystem::path const& path){ return Tag(path); }
		// �v�f
		int index()const{ return index_; }
		filesystem::path path()const;
		types::IYX iHigher() const;// { return{ iY(), iX() }; }
	};
	std::vector<Tag>getAreaTags(std::vector<std::string> const& areaFiles);
		// 2�����b�V���̈���W�͈͎擾
	//  �o��: ((iYa�ŏ�,iXa�ŏ�),(iYa�ő�,iXa�ő�) )
	std::pair<types::IYX, types::IYX  >	extentHigher(std::vector<Tag> const& tags);

	// 50m�~50m�̈���W�͈͎擾(�}�[�W���t)
	//  �o��: ((iYp�ŏ�,iXp�ŏ�),(iYp�ő�,iXp�ő�) )
	std::pair<types::IYX, types::IYX  >	extentLocation(
		filesystem::path const& basePath,
		std::vector<Tag> const& tags);
	// ��r
	inline bool operator==(Tag const& lhs, Tag const& rhs){ return lhs.index() == rhs.index(); }
	inline bool operator!=(Tag const& lhs, Tag const& rhs){ return lhs.index() != rhs.index(); }

	//-------------------------------------------------------------------
	// Mem(2�����b�V��)�������^
	//-------------------------------------------------------------------
	// �� iPos�́A�v���O�������ł͒P��"i"�Ƃ��ėp����B
	//    ���邢�́AiLower���擾���邽�߂�index�Ƃ������o�ŁA
	//    iiLower�Ƃ����肷��B
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
		// ����
		Area(){}
		// Area() = delete;

		explicit Area(filesystem::path const& path);

		// ����
		void read(filesystem::path const& path);
		// �v�f
		//
		Tag tag()const{ return tag_; }
		//(�W���f�[�^)
		short operator()(int iiLower)const
		{
			return data_[iiLower];
		}
		short operator()(types::IYX const& iLower)const
		{
			return data_[iiLower(iLower)];
		}

		// �������̐�
		boost::iterator_range<short const*> lineY(int iY)const
		{ 
			return boost::make_iterator_range_n(&data_[iY*LEN], LEN);
		}
		// �c�����̐�
		boost::range_detail::strided_range<const boost::iterator_range<short const*> >lineX(int iX)const
		{
			return boost::make_iterator_range_n(&data_[iX], LEN*LEN) | adaptors::strided(LEN);
		}
	};
	//-------------------------------------------------------------------
	//  �n�}�t�@�C���p�X�̈ꗗ���擾
	//    (1)���݂̃t�H���_�̋߂��ɂ���"dmap"�t�H���_�̉�������
	//    (2)\����4��\����6��.MEM"�ƂȂ�t�@�C�����擾����B
	//    (3) ����4���͐���6���̏��4���ƈ�v����B
	//-------------------------------------------------------------------
	std::vector<std::string> areaFiles();
	//-------------------------------------------------------------------
	//  �n�}�t�@�C���p�X�̊�b�p�X���擾
	//   �ˊ�b���s���m�ȏꍇ�́A�x���𓊂���
	//-------------------------------------------------------------------
	filesystem::path basePath(std::vector<std::string> const& areaFiles);
	//-------------------------------------------------------------------
	// ���W����
	//   �� iPlace�̑O�A(���_�␳����ĂȂ�)
	//-------------------------------------------------------------------
	int iLocation(int iUpper, int iLower);
	types::IYX iLocation(types::IYX iUpper, types::IYX iLower);


}
