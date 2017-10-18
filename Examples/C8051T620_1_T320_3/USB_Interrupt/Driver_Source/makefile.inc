mofcomp: intusb.bmf

intusb.bmf: intusb.mof
        mofcomp -B:intusb.bmf intusb.mof
        wmimofck intusb.bmf





