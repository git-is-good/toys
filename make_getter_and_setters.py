def make_getter_and_setters(special_starts):
    '''As a decorator, make getter and setter for a class automatically. 
       Example:
        @make_getter_and_setters("_special_")
        class Hashtable(object):
            _special_expandRate = None 
            _special_expandThreshold = None 

        => h = Hashtable().setExpandRate(2).setExpandThreshold(0.75)
           assert h.getExpandRate() == 2
           assert h.getExpandThreshold() == 0.75
    '''
    def _make_getter_and_setters(cls):
        def _make_setter(setter_name, field_name):
            def _setter(self, value):
                setattr(self, field_name, value)
                return self
            _setter.__name__ = setter_name
            return _setter;

        def _make_getter(getter_name, field_name):
            def _getter(self):
                return getattr(self, field_name)
            _getter.__name__ = getter_name 
            return _getter

        # make a copy before iteration
        for k, v in [x for x in cls.__dict__.iteritems()]:
            if k.startswith(special_starts) and v is None:
                field_name = k[len(special_starts):]
                setter_name = "set{}{}".format(field_name[0].upper(), field_name[1:])
                getter_name = "get{}{}".format(field_name[0].upper(), field_name[1:])
                setattr(cls, setter_name,
                        _make_setter(setter_name, field_name))
                setattr(cls, getter_name,
                        _make_getter(getter_name, field_name))
                delattr(cls, k)

        return cls

    return _make_getter_and_setters

def test_make_getter_and_setters():
    @make_getter_and_setters("_special_")
    class Hashtable(object):
        _special_expandRate = None 
        _special_expandThreshold = None 

    h = Hashtable().setExpandRate(2).setExpandThreshold(0.75)
    assert h.getExpandRate() == 2
    assert h.getExpandThreshold() == 0.75

