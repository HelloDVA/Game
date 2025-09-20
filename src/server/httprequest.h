#include<string>
#include<memory>
#include<map>

class HttpRequest {
	public:
		HttpRequest() = default;
		~HttpRequest() = default;

		std::string GetRequest();
	
		// set and get
		std::string getversion() const { return version_;}
		std::string getpath() const { return path_; }
		std::string getmethod() const {return method_; }
		std::string getbody() const { return body_; }
		std::string getheader(std::string key) const { 
			auto it = headers_.find(key);
			if (it != headers_.end()) 
				return it->second;
			else 
				return "";
		}
		void setbody(const std::string& body) { body_ = body; }
		void setversion(const std::string& version){ version_ = version; }
		void setmethod(const std::string& method){ method_ = method; }
		void setpath(const std::string& path){ path_ = path; }
		void setheader(const std::string& key, const std::string& value) { headers_[key] = value; };

	private:
		std::string version_;
		std::string path_;
		std::string method_;
		std::map<std::string, std::string> headers_;
		std::string body_;
};
