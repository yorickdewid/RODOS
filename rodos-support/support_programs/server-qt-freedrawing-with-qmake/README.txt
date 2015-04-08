


client         |    server
               UDP
           Port 6021                                                                      commands/events
RODOS-app ----------->  updToQt.cpp: run() -->update() --> ecuteCommand() --> QTRodosGui <-------------->  QT
                                                                                  |
          <----------- -----------------------------------------------------------+
           Port 6022



