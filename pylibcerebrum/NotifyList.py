
def callback_method(func):
	def notify(self,*args,**kwargs):
		rv = func(self,*args,**kwargs)
		for callback in self.callbacks:
			callback(self)
		return rv
	return notify

#XXX due to lack of proper slice handling, this will *not* work with python < 3.0
class NotifyList(list):
	extend = callback_method(list.extend)
	append = callback_method(list.append)
	remove = callback_method(list.remove)
	pop = callback_method(list.pop)
	__delitem__ = callback_method(list.__delitem__)
	__setitem__ = callback_method(list.__setitem__)
	__iadd__ = callback_method(list.__iadd__)
	__imul__ = callback_method(list.__imul__)

	def __getitem__(self,item):
		if isinstance(item,slice):
			return self.__class__(list.__getitem__(self,item))
		else:
			return list.__getitem__(self,item)

	def __init__(self,*args,callbacks=[]):
		list.__init__(self,*args)
		self.callbacks = callbacks

