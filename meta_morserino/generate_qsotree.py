#!/usr/bin/python3

def seq(*args):
    mixHistoryMaxLength = 0
    ret = ["LIST_SEQ", len(args)]
    for arg in args:
        if(isinstance(arg,list)):
            # do not choke if a tuple comes...
            ret += list(arg)
        elif(isinstance(arg,tuple)):
            ret += arg[0]
            # if we wanted to use the real minimum history length we
            # would use the follow line.
            #mixHistoryMaxLength = max(mixHistoryMaxLength, arg[1])
            mixHistoryMaxLength += arg[1]
        else:
            ret.append(arg)
    return ret, mixHistoryMaxLength

def alt(*args):
    mixHistoryMaxLength = 0
    ret = ["LIST_ALT", len(args)]
    for arg in args:
        if(isinstance(arg,list)):
            ret += arg
        elif(isinstance(arg,tuple)):
            ret += arg[0]
            # if we wanted to use the real minimum history length we
            # would use the follow line.
            #mixHistoryMaxLength = max(mixHistoryMaxLength, arg[1])
            mixHistoryMaxLength += arg[1]
        else:
            ret.append(arg)
    return ret, mixHistoryMaxLength

def prob(probability, arg):
    mixHistoryMaxLength = 0
    # PROB is only allowed to take 1 element!
    # we misuse the "length" byte as a probability.
    ret = ["PROB", int(probability*100),]
    #ret = ["LIST_SEQ", 1, ] ##for debugging, show all
    if(isinstance(arg,list)):
        ret += list(arg)
    elif(isinstance(arg,tuple)):
        ret += arg[0]
        # if we wanted to use the real minimum history length we
        # would use the follow line.
        #mixHistoryMaxLength = max(mixHistoryMaxLength, arg[1])
        mixHistoryMaxLength += arg[1]
    else:
        ret.append(arg)
    return ret, mixHistoryMaxLength

def esc(*args):
    ret = ["ESC_SPECIAL", len(args), ]
    mixHistoryMaxLength = 0
    for arg in args:
        if(isinstance(arg,list)):
            raise ValueError("esc list may only include strings, not nested lists")
            #ret += list(arg)
        elif(isinstance(arg,tuple)):
            raise ValueError("esc list may only include strings, not nested lists")
            #ret += arg[0]
            #mixHistoryMaxLength += arg[1]
        elif(isinstance(arg,str)):
            ret.append(arg)
        else:
            raise ValueError("esc list may only include strings, not nested lists")
        #ret.append(arg)
    return ret, mixHistoryMaxLength

def mix(*args):
    ret = ["LIST_MIX", len(args),]
    mixHistoryMaxLength = len(args)
    for arg in args:
        if(isinstance(arg,list)):
            ret += list(arg)
        elif(isinstance(arg,tuple)):
            ret += arg[0]
            mixHistoryMaxLength += arg[1]
        else:
            ret.append(arg)
    return ret, mixHistoryMaxLength

if(__name__ == "__main__"):
    call1 = esc("CALL1_MAGIC")
    call1x2 = esc("CALL1_MAGIC", "CALL1_MAGIC")
    call2 = esc("CALL2_MAGIC")
    call2x2 = esc("CALL2_MAGIC", "CALL2_MAGIC")
    de = 50
    r = 174
    es = 58
    ur = 215
    _pse = 145
    pse = prob(0.8, _pse)
    k = 99
    my = prob(0.2,126)
    _is = prob(0.2,91)
    op = alt(65, 136, 137, 81, 237, 141)# frd om op ham yl ow*/
    dr = alt(52,104)#dr lbr
    nw = 134
    hr = prob(0.2, 85)
    group5 = esc("GROUPOF5_MAGIC")
    repeat = esc("REPEAT_MAGIC")
    pause = esc("PAUSE_MAGIC")
    stop = esc("BT_IDX")
    ar = esc("AR_IDX")
    c = 70 # this is not numbered in order in the abbrevs
    buro = alt(35,29)
    comma = esc("COMMA_IDX")

    fer = alt(61,64)
    abt = prob(0.2,9)
    cq = 43

    antenna = seq(alt(
        113, #lw
        77, #gp
        15, #dipole
        129, #magloop
        17, #coil
        105, #windom
        98, #zepp
        92, #g5rv
        82, #w3dzz
        109, #log
        seq(prob(0.3,108),219), #vert
        group5,
        seq(esc("NUM1_MAGIC"),59), #123 el
        seq(esc("NUM2_MAGIC"),124), #123 mtr
    ), prob(0.2, seq(214, esc("NUM2_MAGIC"),124))) #up 123 mtr

    sota_chaser_says_bye =seq(alt(203, 204,206), #tnx, tks, tu
                        prob(0.5, seq(3, de,)),#73 de
                        call2, esc("SK_IDX"))

    #
    #
    # you are very welcome to improve this qso tree structure with
    # better, useful and nice qso content examples!
    #
    qso, mixHistoryMaxLength = seq(
        prob(0.2, seq(152, pause, 152, pause)),
        #qrl? qrl?

        #contest, sota, or regular or...?
        alt(
            #regular qso
            seq(
                seq(cq, cq, de, call1x2, alt(seq(pse, k,),ar )),# cq cq
                pause,
                seq(call1, de, call2x2,
                    prob(0.1, seq(pse, alt(170,155,156,158))),# qsy qro qrp qrs
                    alt(seq(pse, k), esc('KN_IDX'))),
                pause,
                ### 1st durchgang
                seq(
                    alt(r,218), # r v
                    alt(68, 71, 72, 74,# ga gd ge gm
                        alt(68,74, 79),#german: ga, gm, gt
                    ),
                    dr,
                    op,
                    es, prob(0.5,alt(224,121,)), alt(203, 204,), fer,
                    # es (vy,mni) (tks,tnx) (fer,fr)
                    alt(18, 37),
                    # ans call
                    stop,
                    mix(
                        seq(prob(0.5,seq(ur, prob(0.2,188))), 183,
                            # ur sigs rst
                            esc("RST_MAGIC"), repeat,
                            prob(0.3,alt(242,60, 210, # plus fb ufb
                                seq(
                                    alt(153, 154, 164, 179, 26),repeat,#qrm qrn qsb rfi bci
                                    #prob(0.2,seq(40,224,28,))#conds vy bd
                                )))
                            ,stop,
                        ),
                        # prob(0.1,seq(prob(0.2,seq(call1, 91, 81, 39, 195,stop)) #is ham cl stn
                        #              ,137, group5, repeat, stop, #op
                        #              prob(0.5, seq(149, group5, repeat, stop)))), #qra
                        seq(my,238,_is, group5, repeat,stop,),
                        # my name is
                        seq(my,172,_is, prob(0.3, 132), esc("NAME1_MAGIC"), esc("NAME1_MAGIC"),
                            prob(0.1, seq(prob(0.5,195), 137, group5)),#op
                            prob(0.1,seq(149, group5, group5, 195))#qra stn
                            ,stop,),
                        # my qth is nr
                        seq(my,239,_is,esc("DOK_MAGIC"),repeat,stop,),
                        # my dok is
                        prob(0.3,seq(my,240,_is,esc("LOCATOR_MAGIC", "REPEAT_MAGIC"),stop,)),
                        # my locator is
                    ),
                    nw,90,esc("QUESTION_IDX"),ar,
                    # nw hw?
                    call2, de, call1,
                    pse, k,
                ),
                pause,
                ### 2. durchgang
                prob(0.2, seq(alt(192,59), hr, 63, esc("AS_IDX", "PAUSE_MAGIC",))),#excus sri hr fone
                seq(call1, de, call2, stop,
                    alt(r, 60, esc("VE_IDX"), # r fb <ve>
                        seq(prob(0.5, 12), 135) #all ok
                    ),
                    dr, op, es,
                    prob(0.5,alt(224,121,)), alt(203, 204,), fer,
                    # vy mni tks tnx fr fer
                    alt(182,182,95,123),stop,#rpt rprt info msg
                    mix(
                        seq(alt(seq(prob(0.5,seq(ur, prob(0.2,188))), 183,),
                                ur),
                            # ur sigs rst
                            esc("RST_MAGIC"), repeat,
                            alt(242, 60, 210, # plus fb ufb
                                seq(prob(0.1, 89),#hvy
                                    alt(153, 154, 164, 179, 26))),#qrm qrn qsb rfi bci
                            stop,
                        ),
                        seq(my,238,_is,group5,repeat,stop,),
                        # my name is
                        seq(my,172,_is,esc("NAME2_MAGIC","NAME2_MAGIC"),stop,),
                        # my qth is
                        seq(my,239,_is,esc("DOK_MAGIC", "REPEAT_MAGIC"),stop,),
                        # my dok is
                        prob(0.10,seq(esc("AS_IDX", "PAUSE_MAGIC", "PAUSE_MAGIC"),)),
                        # make <as> a second break
                        prob(0.3,seq(my,240,_is,esc("LOCATOR_MAGIC", "REPEAT_MAGIC"),stop,)),
                        # my locator is

                    ),
                    mix(
                        seq(hr, alt(
                            seq(alt(180,205,235),group5),#(rig trx xcvr)
                            seq(185, group5, 208, group5)),#rx tx
                            prob(0.1, seq(227,34)), # wid bug
                            prob(0.9,seq(
                                alt(227,es,stop), 147,#pwr
                                abt, alt(seq(esc("NUM3_MAGIC"),alt(225, 226)),#w,wtts
                                         seq(esc("NUM1_MAGIC"), 102),#kw
                                ),
                                prob(0.5,143), #pep
                                alt(147,es,stop),19,group5,stop #ant
                            )),
                        ),
                        seq(233,hr,alt(esc("GROUPOF5_MAGIC","GROUPOF5_MAGIC"), 49, 30, 111, 198, 231, 60, 210), #wx sunny cldy rain warm snow fb ufb
                            200, esc("NUM2_MAGIC"), c, stop, #temp
                        ),
                    ),
                    84, 135, esc("QUESTION_IDX"), ar, #hpe ok ?
                    call1, de, call2, pse, k,
                ),
                pause,
                ### kurze wechsel
                prob(0.1, seq(pse, ur, 19, 14, esc('QUESTION_IDX','BK_IDX'),#ant agn?
                              pause,
                              88, antenna, repeat, hr, esc('BK_IDX'), #hv
                              pause,
                )),
                prob(0.1, seq(pse, ur, 238, 14, esc('QUESTION_IDX','BK_IDX'),#name agn?
                              pause,
                              esc("NAME2_MAGIC", "REPEAT_MAGIC"), esc('BK_IDX'),
                              pause,
                              alt(224,121,),esc("NAME2_MAGIC")
                )),
                prob(0.1, seq(236, 91, ur, 13, esc('QUESTION_IDX','BK_IDX'),#what is ur age ?
                              pause,
                              13, hr, 91, esc('NUM2_MAGIC', "REPEAT_MAGIC",'BK_IDX'), #age hr is
                              pause,
                )),
                ### 3. durchgang
                seq(
                    call2, de, call1, stop,
                    alt(r, 60,esc("VE_IDX"),# r fb
                        seq(prob(0.5, 12), 135) #all ok
                    ),
                    dr, alt(op,esc("NAME2_MAGIC")), es,
                    alt(206, prob(0.5,alt(224,121,)), alt(203, 204,)), fer,
                    # tu / vy mni tks tnx fr fer
                    alt(182,95,123),stop,#rprt info msg
                    prob(0.1,seq(224,146,comma,88,120,228,131,148,36)),#vy psed hv not wkd ur px b4
                    mix(
                        seq(hr, alt(180,205,235),group5,#(rig trx xcvr)
                                prob(0.1, seq(227,alt(34,58,seq(56,103)))), # wid bug elbug (el ky)
                            prob(0.9,seq(
                                alt(227,es,stop), 147,#pwr
                                abt, alt(seq(esc("NUM3_MAGIC"),alt(225, 226)),#w,wtts
                                         seq(esc("NUM1_MAGIC"), 102),#kw
                                ),
                                prob(0.5,143), #pep
                            alt(227,es,stop),19,antenna,stop #ant
                        ))),
                        seq(233, hr, alt(esc("GROUPOF5_MAGIC", "GROUPOF5_MAGIC"), 49, 30, 111, 198, 231), es, #wx sunny cldy rain warm snow
                            200, esc("NUM2_MAGIC"), c, stop, #temp
                        ),
                        seq(33,88,alt(228,86), #btw hv wkd hrd
                            group5,
                            prob(0.5,alt(191,110)), #sp lp
                            alt(36,241),#b4 yday
                            alt(132,151),#nr qrg
                            esc("NUM2_MAGIC"),118, stop #99 mhz

                        ),
                        seq(227,alt(213,197),65,hr,238,esc("GROUPOF5_MAGIC","REPEAT_MAGIC"),stop), #wid unlis frd name
                    ),
                    nw, 160, stop,# qru
                    prob(0.1, seq(230,170,fer,189,prob(0.5,190),stop)),# wl qsy fer fr sked sn
                    mix(
                    seq(_pse, ur, 166, 221, #qsl via
                        alt(buro, seq(117, group5)),#buro mgr
                                      prob(0.5,202) , stop), #tia
                    seq(alt(206, seq(prob(0.5,alt(224,121,)), alt(203, 204,))), fer,
                    # tu / vy mni tks tnx fr fer
                        132, 167), #nice qso
                    seq(es, alt(seq(84, 45,), #hpe cuagn
                            seq(alt(44,46),14),#cu agn
                            27,#bcnu
                            23, 24)), # awdh awds
                    seq(60, 114, 12, esc('NAME2_MAGIC')), #fb on all
                    seq(146, 88, 228, 209) #psed hv wkd u
                    ),
                    seq(prob(0.2, seq(2, es)), # 55 es
                        prob(0.2, 224,), alt(3,4)),# vy (72 73)
                        es, alt(seq(73, 73),# es gl gl
                                69, 75),#gb gn
                    esc("SK_IDX"),
                ),
                pause,
                ### 4. durchgang
                seq(call1, de, call2, stop,
                    alt(r, 60,esc("VE_IDX"),# r fb
                        seq(prob(0.5, 12), 135) #all ok
                    ),
                    dr, alt(op,esc("NAME1_MAGIC")), stop,
                    prob(0.1, seq(42, fer, alt(# congrats fer
                        seq(60, 195), #fb stn
                        107,#lis
                    ))),
                    prob(0.1,seq(160,243,stop)), #qru also
                    alt(seq(166, prob(0.5, 196), 221, buro, prob(0.5,135), stop),
                        #qsl (sure) via (buro bureau) (ok)
                        seq(131, 192, _pse, 59, #no sri pse excus
                            alt(seq(131, 166, buro, hr), #no qsl buro hr
                            ),
                            alt(prob(0.5, seq(_pse, 221, 186)), #pse via sase
                                seq(_pse, 59,166, 221,128)#pse el qsl via net
                            ),
                            stop,)
                    ),
                    alt(206, prob(0.5,alt(224,121,)), alt(203, 204,)), fer,
                    # tu / vy mni tks tnx fr fer
                    prob(0.5, 132),167, # nice qso
                    prob(0.2, seq(2, es)), # 55 es
                    prob(0.2, 224,), alt(3,4), es, #vy (72 73) es
                    alt(69, 75, 73,72, #gb gn gl ge
                        seq(80, 54), #gud dx
                        ),
                    dr, op,
                    es, alt(seq(84, 45,), #hpe cuagn
                            seq(alt(44,46),14),#cu agn
                            27,#bcnu
                            23, 24), # awdh awds
                    prob(0.3, seq(93, 47)),# in cw
                    stop,
                    call1, de, call2,
                    alt(seq(esc("SK_IDX"),
                            pause,seq(206,4), prob(0.5, alt(55,39)), pause,), #tu 73 ee cl
                        seq(alt(55,39)), #ee cl
                        seq(pse, k,pause,
                            206,4,55, pause), #tu 73 ee
                    ),
                ),
            ),
            ###### SOTA qso
            seq(
                #activator calls
                seq(cq, 216, cq, 216, de, call1x2, #cq sota cq sota de call1
                    114, esc("SOTA_MAGIC", "REPEAT_MAGIC"), alt(seq(pse, k), ar)), #on
                pause,
                #chaser calls activator
                seq(call1, de, call2x2,
                    prob(0.1, seq(pse, alt(170,155,156,158))),# qsy qro qrp qrs
                    alt(seq(pse, k), esc('KN_IDX'))),
                pause,
                # caser replies with report
                seq(call2, prob(0.5,seq(ur, prob(0.2,188))), 183,
                    # ur sigs rst
                    esc("RST_MAGIC", "REPEAT_MAGIC", "REPEAT_MAGIC"),
                    de, call1, pse, k),
                pause,
                #chaser confirms
                seq(alt(r, 60, esc("VE_IDX"), # r fb <ve>
                        135), repeat, #ok
                    call2, prob(0.5,seq(ur, prob(0.2,188))), 183,
                    # ur sigs rst
                    esc("RST_MAGIC","REPEAT_MAGIC"),
                    de, call2, pse, k),
                pause,
                #activator acknowledges rprt and signs off
                seq(alt(r, 60, esc("VE_IDX"), # r fb <ve>
                        135), repeat, #ok
                    alt(203, 204,206), #tnx, tks, tu
                    3, de, call1),#73
                ####alt(
                    seq(alt(55, # ee
                            seq(ar, k),),
                        pause,
                        sota_chaser_says_bye,
                        pause,
                        # If the chaser sends TU 73 etc the activato
                        # r may send a final final, pause and then call  briefly in
                        # anticipation of a pile up.
                        seq(55, cq, 216, de, call1, k),
                        pause,
                ),
                #     # activator ends activation.
                #     seq(esc("SK_IDX"),39,#cl
                #         pause,
                #         sota_chaser_says_bye,
                #         pause
                #     )
                # ),
            ),
            ####### contest qso
            seq(prob(0.5, cq), 201, call1, call1, k, #test
                esc("PAUSE_MAGIC",
                    "CALL2_MAGIC"), k,
                esc("PAUSE_MAGIC",
                    "CALL2_MAGIC","RST_MAGIC", "NUM3_MAGIC"), k,
                pause,
                prob(0.2, seq(133, 14, 145,#nr agn pse
                              esc("PAUSE_MAGIC",
                              "NUM3_MAGIC", "REPEAT_MAGIC"), k,)),
                r, prob(0.5, ur), esc("RST_MAGIC", "NUM3_MAGIC"),
                alt(k,
                    seq(esc("REPEAT_MAGIC","QUESTION_IDX","BK_IDX",
                            "PAUSE_MAGIC"),
                        r,alt(70,r),esc("BK_IDX","PAUSE_MAGIC"))), # c
                pause,
                alt(203, 204,206), #tnx, tks, tu
                201, call1x2 #test
                )
        ),
    )

    dbgqso, dbgMixHistoryMaxLength = seq(es,0,1,
                  esc("CALL1_MAGIC"),
                  esc("CALL2_MAGIC"),
                  # esc("NAME1_MAGIC"),
                  # esc("NAME2_MAGIC"),
                  # esc("RANDCALL_MAGIC"),
                  # esc("RANDCALL_MAGIC"),
                  # esc("RST_MAGIC"),
                  # esc("REPEAT_MAGIC"),
                  # esc("PAUSE_MAGIC"),
                  # esc("NUM1_MAGIC"),
                  # esc("NUM2_MAGIC"),
                  # esc("NUM3_MAGIC"),
                  # esc("GROUPOF5_MAGIC"),
                  # esc("LOCATOR_MAGIC"),
                  # esc("DOK_MAGIC"),
                  # esc("AS_IDX"),
                  # esc("SK_IDX"),
    )

    # check for elements larger than NUMBER_OF_ABBREVS
    toobig = [elem for elem in qso if isinstance(elem, int) and elem>= 244]
    if (len(toobig) > 0):
        print(toobig)
        raise ValueError("There are values outside the abbrev index range.")
    with open("qsotree.h","w") as f:
        print("#ifndef QSOTREE_H",file=f)
        print("#define QSOTREE_H",file=f)
        print("const byte qsotree[%d] PROGMEM = {" % len(qso),file=f)
        print("#ifdef DEBUG",file=f)
        print(", ".join([str(elem) for elem in dbgqso]),file=f)
        print("#else",file=f)
        print(", ".join([str(elem) for elem in qso]),file=f)
        print("#endif",file=f)
        print("};",file=f)
        print("// a list which will contain the indices of the mix elements ",file=f)
        print("// which have been printed already.",file=f)
        print("#ifdef DEBUG",file=f)
        print("uint16_t mixHistory[%d];" % dbgMixHistoryMaxLength,file=f)
        print("#else",file=f)
        print("uint16_t mixHistory[%d];" % mixHistoryMaxLength,file=f)
        print("#endif",file=f)
        print("#endif",file=f)
