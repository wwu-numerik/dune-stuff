#ifndef DUNE_FEM_HELPERCLASS_HH
#define DUNE_FEM_HELPERCLASS_HH

#include <sstream>
#include <assert.h>
#include <dune/grid/common/gridinfo.hh>
#include <dune/fem/space/common/dofmanager.hh>

// GridType 
// ----------------------
#if DUNE_VERSION_NEWER(DUNE_GRID,2,1,0)
#include <dune/grid/utility/gridtype.hh>
//#include <gridtype.hh>
#else
#include <dune/grid/io/file/dgfparser/dgfgridtype.hh>
#endif

//foreground colors
#define	_BLACK << "\033[30m" <<
#define	_RED << "\033[31m" <<
#define	_GREEN << "\033[32m" <<
#define	_BROWN << "\033[33m" <<
#define	_BLUE << "\033[34m" <<
#define	_PURPLE << "\033[35m" <<
#define	_CYAN << "\033[36m" <<
#define	_LIGHTGRAY << "\033[37m" <<
//light foreground colors
#define	_DARKGRAY << "\033[1;30m" <<
#define	_LIGHTRED << "\033[1;31m" <<
#define	_LIGHTGREEN << "\033[1;32m" <<
#define	_YELLOW << "\033[1;33m" <<
#define	_LIGHTBLUE << "\033[1;34m" <<
#define	_LIGHTPURPLE << "\033[1;35m" <<
#define	_LIGHTCYAN << "\033[1;36m" <<
#define	_WHITE << "\033[1;37m" <<
//background colors
#define	_BBLACK << "\033[40m" <<
#define	_BRED << "\033[41m" <<
#define	_BGREEN << "\033[42m" <<
#define	_BBROWN << "\033[43m" <<
#define	_BBLUE << "\033[44m" <<
#define	_BPURPLE << "\033[45m" <<
#define	_BCYAN << "\033[46m" <<
#define	_BLIGHTGRAY << "\033[47m" <<
//back foreground colors
#define	_BDARKGRAY << "\033[1;40m" <<
#define	_BLIGHTRED << "\033[1;41m" <<
#define	_BLIGHTGREEN << "\033[1;42m" <<
#define	_BYELLOW << "\033[1;43m" <<
#define	_BLIGHTBLUE << "\033[1;44m" <<
#define	_BLIGHTPURPLE << "\033[1;45m" <<
#define	_BLIGHTCYAN << "\033[1;46m" <<
#define	_BWHITE << "\033[1;47m" <<
//modifiers
#define	_NORMAL << "\033[0m" <<
#define	_BOLD << "\033[1m" <<
#define	_ITALIC << "\033[2m" <<
#define	_UNDERLINE << "\033[4m" <<
#define	_BLINK << "\033[5m" <<
#define	_REVERSE << "\033[7m" <<
#define	_ENDITALIC << "\033[22m" <<
#define	_ENDUNDERLINE << "\033[24m" <<
#define	_ENDBLINK << "\033[25m" <<
#define	_ENDREVERSE << "\033[27m" <<
//general color
#define _COLOR(x) << "\033[38;5;"x"m" <<
#define _BCOLOR(x) << "\033[48;5;"x"m" <<


//foreground colors
#define	BLACK "\033[30m" 
#define	RED "\033[31m" 
#define	GREEN "\033[32m" 
#define	BROWN "\033[33m" 
#define	BLUE "\033[34m" 
#define	PURPLE "\033[35m" 
#define	CYAN "\033[36m" 
#define	LIGHTGRAY "\033[37m"
//light foreground colors
#define	DARKGRAY "\033[1;30m" 
#define	LIGHTRED "\033[1;31m" 
#define	LIGHTGREEN "\033[1;32m" 
#define	YELLOW "\033[1;33m" 
#define	LIGHTBLUE "\033[1;34m" 
#define	LIGHTPURPLE "\033[1;35m" 
#define	LIGHTCYAN "\033[1;36m" 
#define	WHITE "\033[1;37m" 
//background colors
#define	BBLACK "\033[40m" 
#define	BRED "\033[41m" 
#define	BGREEN "\033[42m" 
#define	BBROWN "\033[43m" 
#define	BBLUE "\033[44m" 
#define	BPURPLE "\033[45m"
#define	BCYAN "\033[46m" 
#define	BLIGHTGRAY "\033[47m" 
//back foreground colors
#define	BDARKGRAY "\033[1;40m" 
#define	BLIGHTRED "\033[1;41m" 
#define	BLIGHTGREEN "\033[1;42m" 
#define	BYELLOW "\033[1;43m" 
#define	BLIGHTBLUE "\033[1;44m" 
#define	BLIGHTPURPLE "\033[1;45m" 
#define	BLIGHTCYAN "\033[1;46m" 
#define	BWHITE "\033[1;47m" 
//modifiers
#define	NORMAL "\033[0m" 
#define	BOLD "\033[1m" 
#define	ITALIC "\033[2m" 
#define	UNDERLINE "\033[4m" 
#define	BLINK "\033[5m" 
#define	REVERSE "\033[7m" 
#define	ENDITALIC "\033[22m" 
#define	ENDUNDERLINE "\033[24m" 
#define	ENDBLINK "\033[25m" 
#define	ENDREVERSE "\033[27m" 
//general color
#define COLOR(x) "\033[38;5;"x"m" 
#define BCOLOR(x) "\033[48;5;"x"m" 

//some additional stuff
#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include <string>

//converts int to string by using ostringstream
std::string intToString(int i){
	std::ostringstream out;
	out << i;
	return out.str();
}

//demangles typeid
template<class T>
std::string demangledTypeId(T& obj){
	int status;
#ifdef __GNUC__
	return abi::__cxa_demangle(typeid(obj).name(), 0, 0, &status);	
#else
	return typeid(obj).name();
#endif
}

//create output for demangled typeid
template<class T>
void realTypeId(T& obj, std::string name="", int maxlevel=10000){
	std::cout << name << (name=="" ? "" : "'s type is ") << highlightTemplate(demangledTypeId(obj),maxlevel)  <<  std::endl ;	
}

//maybe you want to choose your own color
int templateColorChooser(int i){
	assert(i<256 && i>=0);
	//return i*2+17
	return i;
}

//highlight templates depending on the "template"-level
std::string highlightTemplate(std::string str, int maxlevel = 10000){
	assert(maxlevel>=0);
	int startindex = 0;
	int level=0;
	for (size_t i=0; i<str.size(); i++){
		if (str[i] == '<'){
			level++;
			std::string dummy = "\033[38;5;"+ intToString(templateColorChooser(level))+"m";
			str.insert(i,dummy);
			i+=dummy.size();
			if (level == maxlevel)
				startindex=i+1;
		}
		else if (str[i] == '>'){
			level--;			
			std::string dummy = "\033[38;5;"+ intToString(templateColorChooser(level))+"m";
			str.insert(++i,dummy);
			if (level+1 == maxlevel){
				int size=i-startindex-1;
				str.erase(startindex, size);
				i=startindex+1;
			}
			i+=dummy.size();
		}
	}
	str+="\033[38;5;0m";
	return str;
}

//highlight a string in a specified color
std::string highlightString(std::string str, int colornr=0){
	assert(colornr<256 && colornr>=0);
	return "\033[38;5;"+intToString(colornr)+"m"+str+"\033[38;5;0m";

}

//highlight a string which is searched in another string in a specified color
std::string highlightSearchString(std::string str, std::string searchstr, int colornr=0){
	assert(colornr<256 && colornr>=0);
	int index=str.find(searchstr,0);
	while (index!=int(std::string::npos)){
		std::string dummy = "\033[38;5;"+intToString(colornr)+"m";
		std::string dummy2 = "\033[38;5;0m";
		str.insert(index, dummy);
		str.insert(index+searchstr.size()+dummy.size(), dummy2);
		index=str.find(searchstr,index+dummy.size()+searchstr.size()+dummy2.size());
	}
	return str;
}

//an info printer class, designed for printing small output
//only for one object
class StaticInfoPrinter
{
#if DUNE_VERSION_NEWER(DUNE_GRID,2,1,0)
	typedef Dune::GridSelector::GridType   MyHGridType ;
#else
	typedef GridType    MyHGridType ;
#endif

	public:

	template <class FunctionSpaceImp>
	static void functionSpaceInfo(const FunctionSpaceImp& dfSpace){
		header("function space information");
		std::cout << "type(identifier) = " << dfSpace.type() << std::endl;
		for(int i = 0; i<= MyHGridType::dimension;i++){
			std::cout << "codim " << i << " = " << dfSpace.contains(i) << ", ";
		}
		std::cout << std::endl;
		std::cout << "sequence = " << dfSpace.sequence() << std::endl;
		std::cout << "continuous = " << dfSpace.continuous() << std::endl;
		std::cout << "order = " << dfSpace.order() << std::endl;
		std::cout << "size = " << dfSpace.size() << std::endl;
		std::cout << "multipleGeometryTypes = " << dfSpace.multipleGeometryTypes() << std::endl;
		std::cout << "multipleBaseFunctionSets = " << dfSpace.multipleBaseFunctionSets() << std::endl;
		std::cout << "maxNumLocalDofs = " << dfSpace.maxNumLocalDofs() << std::endl;
	}

	template <class MapperTypeImp>
	static void mapperInfo(const MapperTypeImp& mapper){
		header("mapper information");
		std::cout << "size = " << mapper.size() << std::endl;
		for(int i = 0; i<= MyHGridType::dimension;i++){
			std::cout << "codim " << i << " = " << mapper.contains(i) << ", ";
		}
		std::cout << std::endl;
		std::cout << "numBlocks = " << mapper.numBlocks() << std::endl;
		std::cout << "consecutive = " << mapper.consecutive() << std::endl;
		std::cout << "blocks: ";
		for(int i=0; i<mapper.numBlocks(); i++){
			std::cout << "numberOfHoles = " << mapper.numberOfHoles(i)
								<< ", offSet = " << mapper.offSet(i)
								<< std::endl;
		}	
	}

	template <class BlockMapperTypeImp>
	static void blockMapperInfo(const BlockMapperTypeImp& blockMapper){
		header("block mapper information");
		std::cout << "size = " << blockMapper.size() << std::endl;
		for(int i = 0; i<= MyHGridType::dimension;i++){
			std::cout << "codim " << i << " = " << blockMapper.contains(i) << ", ";
		}
		std::cout << std::endl;
		std::cout << "numBlocks = " << blockMapper.numBlocks() << std::endl;
		std::cout << "consecutive = " << blockMapper.consecutive() << std::endl;
		std::cout << "blocks: ";
		for(int i=0; i<blockMapper.numBlocks(); i++){
			std::cout << "numberOfHoles = " << blockMapper.numberOfHoles(i)
								<< ", offSet = " << blockMapper.offSet(i)
								<< std::endl;
		}
	}

	template <class LagrangePointSetImp, class DofInfoImp>
	static void lagrangePointTypeInfo(const LagrangePointSetImp& set, int numdofs, const DofInfoImp& ){
		header("lagrange point type information");
		for (int i=0; i<numdofs; ++i){
			const DofInfoImp& dofInfo = set->dofInfo(i);
			std::cout << "local number: " << i << ", codim: " << dofInfo.codim << ", subEntity: " << dofInfo.subEntity << ", dofNumber: " << dofInfo.dofNumber << std::endl; 
		}
	}


private:
	static void header(std::string name=""){
		std::cout << "==========" << (name==""? "" : " ") << name << (name==""? "" : " ") << "=================================================" << std::endl;
	}

	void smallSeparator(std::string name=""){		
		std::cout << "---------" << name << "----------------------------------------------------------------------------" << std::endl;
	}

};

//an info printer class printing a lot of information.
//This class is designed for printing all information that can
//be extraced from a discrete function
template<class DiscFuncType>
class InfoPrinter
{
#if DUNE_VERSION_NEWER(DUNE_GRID,2,1,0)
	typedef Dune::GridSelector::GridType    MyHGridType ;
#else 
	typedef GridType   MyHGridType ;
#endif

	public:

	typedef typename DiscFuncType::GridType                      GGridType;
	typedef typename DiscFuncType::DiscreteFunctionSpaceType     DiscSpaceType;
	typedef typename GGridType::template Codim<0>::LeafIterator   LeafIteratorType;
	typedef typename GGridType::template Codim<1>::LeafIterator   IntersectionLeafIteratorType;
	typedef typename GGridType::template Codim<2>::LeafIterator   IntersectionLeafIteratorType2;

	typedef typename DiscSpaceType::BaseFunctionSetType          BaseFunctionSetType;
	typedef typename DiscSpaceType::IteratorType                 IteratorType;
	typedef typename DiscSpaceType::IndexSetType                 IndexSetType;
	typedef typename IteratorType::Entity                        EntityType;
	typedef typename EntityType::Geometry                        GeometryType;
	typedef typename GeometryType::LocalCoordinate               LocalCoordinateType;
	typedef typename GeometryType::GlobalCoordinate              GlobalCoordinateType;
	typedef typename GeometryType::ctype                         ctype;
	typedef typename DiscFuncType::LocalFunctionType             LocalFunctionType;
	typedef Dune::DofManager<GGridType>                           DofManagerType;
	typedef typename GGridType::GlobalIdSet                       GlobalIdSet;
	typedef typename GGridType::LocalIdSet                        LocalIdSet;
	typedef typename GGridType::LevelIndexSet                     LevelIndexSet;
	typedef typename GGridType::LeafIndexSet                      LeafIndexSet;
	

	typedef typename GGridType::LeafGridView                      LeafGridView;
	typedef typename GGridType::LevelGridView                     LevelGridView;


	typedef typename LeafGridView::template Codim<0>::Iterator   LeafIterator; //element
	typedef typename LeafGridView::template Codim<1>::Iterator   LeafIterator1;
	typedef typename LeafGridView::template Codim<2>::Iterator   LeafIterator2; //vertex for dim=2
	typedef typename LevelGridView::template Codim<0>::Iterator  LevelIterator;
	typedef typename LevelGridView::template Codim<1>::Iterator  LevelIterator1;
	typedef typename LevelGridView::template Codim<2>::Iterator  LevelIterator2;


	InfoPrinter(DiscFuncType& df)
		: df_(df),
		  discfuncspace_(df.space()),
			endit_(discfuncspace_.end()),
			grid_(discfuncspace_.grid()),
			dofman_(DofManagerType::instance(grid_))
	{
		realTypeId(df_, "discrete function");
		realTypeId(discfuncspace_, "discrete function space");
		realTypeId(endit_, "discrete function space iterator");
		realTypeId(grid_, "grid");
		realTypeId(dofman_, "dofmanager");
	}

	void indexSetInfo(){
		header("index set information");
		const IndexSetType& indexSet = discfuncspace_.gridPart().indexSet();
		LeafGridView leafView = grid_.leafView();
		for(IteratorType it = discfuncspace_.begin(); it!=endit_; ++it) {
			const EntityType& en = *it;
			LocalFunctionType localfunc = df_.localFunction(en);
			localfunc.init(en);

			int number = indexSet.index(en);
			std::cout << "index = " << number << ": "; 
			std::cout << "codim " << 0 << ": ";
			for(int ii=0; ii<it->template count<0>() ; ii++){
				std::cout << indexSet.subIndex(en,ii,0) << ", ";
			}
			if (MyHGridType::dimension >= 1){
				std::cout << "codim " << 1 << ": ";
				for(int ii=0; ii<it->template count<1>() ; ii++){
					std::cout << indexSet.subIndex(en,ii,1) << ", ";
				}
			}
			if (MyHGridType::dimension >= 2){
				std::cout << "codim " << 2 << ": ";
				for(int ii=0; ii<it->template count<2>() ; ii++){
					std::cout << indexSet.subIndex(en,ii,2) << ", ";
				}
			}
			if (MyHGridType::dimension >= 3){
				std::cout << "codim " << 3 << ": ";
				for(int ii=0; ii<it->template count<3>() ; ii++){
					std::cout << indexSet.subIndex(en,ii,3) << ", ";
				}
			}
			std::cout << std::endl;
		}
	}

	void functionSpaceInfo(){
		header("function space information");
		std::cout << "type(identifier) = " << discfuncspace_.type() << std::endl;
		for(int i = 0; i<= MyHGridType::dimension;i++){
			std::cout << "codim " << i << " = " << discfuncspace_.contains(i) << ", ";
		}
		std::cout << std::endl;
		std::cout << "sequence = " << discfuncspace_.sequence() << std::endl;
		std::cout << "continuous = " << discfuncspace_.continuous() << std::endl;
		std::cout << "order = " << discfuncspace_.order() << std::endl;
		std::cout << "size = " << discfuncspace_.size() << std::endl;
		std::cout << "multipleGeometryTypes = " << discfuncspace_.multipleGeometryTypes() << std::endl;
		std::cout << "multipleBaseFunctionSets = " << discfuncspace_.multipleBaseFunctionSets() << std::endl;
		std::cout << "maxNumLocalDofs = " << discfuncspace_.maxNumLocalDofs() << std::endl;
	}

	void discreteFunctionInfo(){
		header("discrete function information");
		std::cout << "name = " << df_.name() << std::endl;
	}

	void gridInfo(){
		LeafGridView leafView = grid_.leafView();
		LevelGridView levelView = grid_.levelView(0);
		header("grid information");
		//std::cout << "name = " << grid_.name() << std::endl;
	  //my small grid walk
		std::cout << "codim 0 iteration" << std::endl;
		LeafIterator leafend0 = leafView.template end<0>();
		int uu=0;
		for(LeafIterator it = leafView.template begin<0>(); it != leafend0; ++it){
			for(int i=0; i<it->geometry().corners(); i++){
				std::cout << "["<< it->geometry().corner(i) << "], ";
			}
			std::cout << std::endl;
			uu++;
		}
		std::cout << uu << " iterations done" << std::endl;
		std::cout << "codim 1 iteration" << std::endl;
		LeafIterator1 leafend1 = leafView.template end<1>();
		uu=0;
		for(LeafIterator1 it = leafView.template begin<1>(); it != leafend1; ++it){
			for(int i=0; i<it->geometry().corners(); i++){
				std::cout << "["<< it->geometry().corner(i) << "], ";
			}
			std::cout << std::endl;
			uu++;
		}
		std::cout << uu << " iterations done" << std::endl;
		std::cout << "codim 2 iteration" << std::endl;
		LeafIterator2 leafend2 = leafView.template end<2>();
		uu=0;
		for(LeafIterator2 it = leafView.template begin<2>(); it != leafend2; ++it){
			for(int i=0; i<it->geometry().corners(); i++){
				std::cout << "["<< it->geometry().corner(i) << "], ";
			}
			std::cout << std::endl;
			uu++;
		}	
		std::cout << uu << " iterations done" << std::endl;
		smallSeparator();
		Dune::gridinfo(grid_, "##   ");
		for(int i=0; i<=grid_.maxLevel(); i++){
			smallSeparator();
			Dune::gridlevellist(grid_,i,"##   ");
		}
		smallSeparator();
		Dune::gridleaflist(grid_, "##   ");
	}

	void mapperInfo(){
		header("mapper information");
		std::cout << "size = " << discfuncspace_.mapper().size() << std::endl;
		for(int i = 0; i<= MyHGridType::dimension;i++){
			std::cout << "codim " << i << " = " << discfuncspace_.mapper().contains(i) << ", ";
		}
		std::cout << std::endl;
		std::cout << "numBlocks = " << discfuncspace_.mapper().numBlocks() << std::endl;
		std::cout << "consecutive = " << discfuncspace_.mapper().consecutive() << std::endl;
		for(IteratorType it = discfuncspace_.begin(); it!=endit_; ++it) {
			const EntityType& en = *it;
			LocalFunctionType localfunc = df_.localFunction(en);
			localfunc.init(en);

			std::cout << "numEntityDofs = " << discfuncspace_.mapper().numEntityDofs(en) 
								<< ", numDofs = " << discfuncspace_.mapper().numDofs(en)
								<< ", mapToGlobal = "; 
			for(int i=0; i<localfunc.numDofs() ; i++){
				std::cout << discfuncspace_.mapper().mapToGlobal(en,i) << ", ";
			}
			std::cout << std::endl;
		}
		std::cout << "blocks: ";
		for(int i=0; i<discfuncspace_.mapper().numBlocks(); i++){
			std::cout << "numberOfHoles = " << discfuncspace_.mapper().numberOfHoles(i)
								<< ", offSet = " << discfuncspace_.mapper().offSet(i)
								<< std::endl;
		}
	}

	void blockMapperInfo(){
		header("block mapper information");
		std::cout << "size = " << discfuncspace_.blockMapper().size() << std::endl;
		for(int i = 0; i<= MyHGridType::dimension;i++){
			std::cout << "codim " << i << " = " << discfuncspace_.blockMapper().contains(i) << ", ";
		}
		std::cout << std::endl;
		std::cout << "numBlocks = " << discfuncspace_.blockMapper().numBlocks() << std::endl;
		std::cout << "consecutive = " << discfuncspace_.blockMapper().consecutive() << std::endl;
		for(IteratorType it = discfuncspace_.begin(); it!=endit_; ++it) {
			const EntityType& en = *it;
			LocalFunctionType localfunc = df_.localFunction(en);
			localfunc.init(en);

			std::cout << "numEntityDofs = " << discfuncspace_.blockMapper().numEntityDofs(en); 
			std::cout	<< ", numDofs = " << discfuncspace_.blockMapper().numDofs(en);
			std::cout	<< ", mapToGlobal = "; 
			for(int i=0; i<discfuncspace_.blockMapper().numDofs(en) ; i++){
				std::cout << discfuncspace_.blockMapper().mapToGlobal(en,i) << ", ";
			}
			std::cout << std::endl;
		}
		std::cout << "blocks: ";
		for(int i=0; i<discfuncspace_.blockMapper().numBlocks(); i++){
			std::cout << "numberOfHoles = " << discfuncspace_.blockMapper().numberOfHoles(i)
								<< ", offSet = " << discfuncspace_.blockMapper().offSet(i)
								<< std::endl;
		}
	}

	void elementInfo(){
		header("element information");
		for(IteratorType it = discfuncspace_.begin(); it!=endit_; ++it) {
			const EntityType& en = *it;
			LocalFunctionType localfunc = df_.localFunction(en);
			localfunc.init(en);

			const GeometryType& geo = en.geometry();

			std::cout << "entity infos: codim = " << en.codimension 
								<< ", dim = " << en.dimension 
								<< ", mydim = " << en.mydimension 
								<< ", dimworld = " << en.dimensionworld
								<< ", level = " << en.level()							
								<<  std::endl;	

			std::cout << "geometry infos(volume = " << geo.volume()
								<< ", type = " << geo.type() 
								<< ", center = [" << geo.center() << "]): corners = ";
			for(int i=0; i<geo.corners(); i++){
				std::cout << "["<< geo.corner(i) << "], ";
			}
			std::cout <<  std::endl;
			std::cout << "Dofs(numDofs = " << localfunc.numDofs()
								<< ", numScalarDofs = " << localfunc.numScalarDofs() 	
								<< ", polorder = " << localfunc.order() << "): ";
			for(int i=0; i<localfunc.numDofs(); i++){
				std::cout << localfunc[i] << " ";
			}
			std::cout << std::endl;
			smallSeparator();
		}

	}

	void info(){
		DofManagerType::write(grid_,"dofmanager",0);
		std::cout << "hasIndexSets = " << dofman_.hasIndexSets() << std::endl;		

		indexSetInfo();
		discreteFunctionInfo();	
		functionSpaceInfo();
		//gridInfo();
		mapperInfo();
		blockMapperInfo();
		elementInfo();

		std::cout << "==================================================================================" << std::endl;
		std::cout << std::endl << std::endl;
	}

	private:

	void header(std::string name=""){
		std::cout << "========== " << name << " =================================================" << std::endl;
	}

	void smallSeparator(std::string name=""){		
		std::cout << "---------" << name << "----------------------------------------------------------------------------" << std::endl;
	}

	DiscFuncType& df_;
	const DiscSpaceType& discfuncspace_; 
	const IteratorType endit_;
	const GGridType& grid_;
	const DofManagerType& dofman_;

};

#endif // #ifndef DUNE_FEM_HELPERCLASS_HH
