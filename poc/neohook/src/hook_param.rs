// Hook parameters - defines how to extract text from hooked functions
// Based on src/hook_param.h and src/hook_param.cpp

use crate::common::Address;
use regex::Regex;

/// Text encoding type
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Encoding {
    /// Shift-JIS encoding (common in Japanese games)
    ShiftJIS,
    /// UTF-8 encoding
    UTF8,
    /// UTF-16 encoding
    UTF16,
}

impl Encoding {
    pub fn as_str(&self) -> &'static str {
        match self {
            Encoding::ShiftJIS => "shift_jis",
            Encoding::UTF8 => "utf-8",
            Encoding::UTF16 => "utf-16",
        }
    }
}

/// Hook parameter - defines how to extract text from a hooked function
#[derive(Debug, Clone)]
pub struct HookParam {
    /// Hook name/identifier
    pub name: String,
    
    /// Target function address (0 means search by pattern)
    pub address: Address,
    
    /// Encoding of the text
    pub encoding: Encoding,
    
    /// Offset from base to get text pointer
    pub text_offset: isize,
    
    /// Offset from base to get context (for deduplication)
    pub context_offset: Option<isize>,
    
    /// Length offset (if text length is stored separately)
    pub length_offset: Option<isize>,
    
    /// Filter regex pattern (to filter out unwanted text)
    pub filter: Option<Regex>,
    
    /// Whether the text pointer is indirect (pointer to pointer)
    pub indirect: bool,
}

impl HookParam {
    /// Create a new hook parameter
    pub fn new(name: impl Into<String>, address: Address, encoding: Encoding) -> Self {
        Self {
            name: name.into(),
            address,
            encoding,
            text_offset: 0,
            context_offset: None,
            length_offset: None,
            filter: None,
            indirect: false,
        }
    }

    /// Set text offset
    pub fn with_text_offset(mut self, offset: isize) -> Self {
        self.text_offset = offset;
        self
    }

    /// Set context offset
    pub fn with_context_offset(mut self, offset: isize) -> Self {
        self.context_offset = Some(offset);
        self
    }

    /// Set length offset
    pub fn with_length_offset(mut self, offset: isize) -> Self {
        self.length_offset = Some(offset);
        self
    }

    /// Set filter pattern
    pub fn with_filter(mut self, pattern: &str) -> Result<Self, regex::Error> {
        self.filter = Some(Regex::new(pattern)?);
        Ok(self)
    }

    /// Set indirect flag
    pub fn with_indirect(mut self, indirect: bool) -> Self {
        self.indirect = indirect;
        self
    }

    /// Get text address from base address
    pub fn get_text_address(&self, base: Address) -> Address {
        let addr = (base as isize + self.text_offset) as Address;
        
        if self.indirect {
            // Read pointer from address
            unsafe { *(addr as *const Address) }
        } else {
            addr
        }
    }

    /// Get context address from base address (for deduplication)
    pub fn get_context_address(&self, base: Address) -> Option<Address> {
        self.context_offset.map(|offset| {
            (base as isize + offset) as Address
        })
    }

    /// Get text length from base address
    pub fn get_text_length(&self, base: Address, text_addr: Address) -> usize {
        if let Some(length_offset) = self.length_offset {
            // Length is stored at a specific offset
            let length_addr = (base as isize + length_offset) as Address;
            unsafe { *(length_addr as *const usize) }
        } else {
            // Calculate length by finding null terminator
            match self.encoding {
                Encoding::ShiftJIS | Encoding::UTF8 => {
                    // Single-byte null terminator
                    let mut len = 0;
                    unsafe {
                        let ptr = text_addr as *const u8;
                        while *ptr.add(len) != 0 {
                            len += 1;
                            if len > 10000 {
                                break; // Safety limit
                            }
                        }
                    }
                    len
                }
                Encoding::UTF16 => {
                    // Two-byte null terminator
                    let mut len = 0;
                    unsafe {
                        let ptr = text_addr as *const u16;
                        while *ptr.add(len) != 0 {
                            len += 1;
                            if len > 10000 {
                                break; // Safety limit
                            }
                        }
                    }
                    len * 2 // Return byte length
                }
            }
        }
    }

    /// Check if text should be filtered out
    pub fn should_filter(&self, text: &str) -> bool {
        if let Some(filter) = &self.filter {
            filter.is_match(text)
        } else {
            false
        }
    }
}