namespace custom {

  template<class...Funcs>
  struct overloaded : Funcs... { using Funcs::operator()...; };

  template<class...Funcs>
  overloaded(Funcs...)->overloaded<Funcs...>;

  template<class Func>
  Func&& as_obj( Func&& f ){ return forward<Func>(f); }

  template<class R, class...Args>
  auto as_obj( R(*f)(Args...) {
    struct helper {
      R(*f)(Args...);
      R operator()(Args...args) const { return f(forward<Args>(args)...); }
    };
    return helper{f};
  }

  template<class...Funcs>
  auto join( Funcs&&...fs ){
    return overloaded{as_obj(forward<Funcs>(fs))...};
  }
}