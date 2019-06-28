ps -ef | grep mergeServer | grep -v grep | awk '{print $2}' | xargs kill -9
