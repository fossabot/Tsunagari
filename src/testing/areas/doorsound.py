def playSnd(filename):
    snd = resourcer.getSound(filename)
    if snd: # snd will be None if game is run without audio
        snd.play()

playSnd("sounds/door.oga") # closing sound
