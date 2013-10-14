include(CheckCXXSourceCompiles)

check_cxx_source_compiles("struct IFoo
      {
        virtual ~IFoo() {}
        virtual void bar() = 0;
      };

      struct Foo
        : public IFoo
      {
        virtual void bar() override {}
      };
      int main(){}"
  DS_OVERRIDE)

if(DS_OVERRIDE)
    set(DS_OVERRIDE override)
else()
    set(DS_OVERRIDE /*override*/)
endif(DS_OVERRIDE)

check_cxx_source_compiles("struct IFoo
      {
        virtual ~IFoo() {}
        virtual void bar() = 0;
      };

      struct Foo
        : public IFoo
      {
        virtual void bar() final {}
      };
      int main(){}
      "
  DS_FINAL)

if(DS_FINAL)
    set(DS_FINAL final)
else()
    set(DS_FINAL /*final*/)
endif(DS_FINAL)