class singleton(type):
    """
    to create custom singleton class put this as metaclass arg to new class
    """
    _instances = {}
    def __call__(ston, *args, **kwargs):
        if ston not in ston._instances:
            inst = super().__call__(*args, **kwargs)
            ston._instances[ston] = inst
        return ston._instances[ston]