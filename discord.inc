stock SendDiscordMessage(channel, message[]) {
	new DCC_Channel:ChannelId;
	switch(channel)
	{
		//==[ channel register ]
		case 0:
		{
			ChannelId = DCC_FindChannelById("1258013441986265170");
			DCC_SendChannelMessage(ChannelId, message);
			return 1;
		}
		//==[ setup bot ]
		case 1:
		{
			ChannelId = DCC_FindChannelById("1258013440992215161");
			DCC_SendChannelMessage(ChannelId, message);
			return 1;
		}
	}
	return 1;
}

forward DCC_DM_EMBED(str[], pin, id[]);
public DCC_DM_EMBED(str[], pin, id[])
{
    new DCC_Channel:PM, query[200];
	PM = DCC_GetCreatedPrivateChannel();

	new DCC_Embed:embed = DCC_CreateEmbed(.title="Registered Warga", .image_url="https://i.postimg.cc/TYdRBM6q/kkrp.png");
	new str1[100], str2[100];

	format(str1, sizeof str1, "Selamat!\n\nUCP kamu berhasil terverifikasi,\n\nGunakan PIN dibawah ini untuk login ke Dalam Game");
	DCC_SetEmbedThumbnail(embed, "https://i.postimg.cc/y81xyyMV/kotakito.png");
	DCC_SetEmbedDescription(embed, str1);
	format(str1, sizeof str1, "UCP");
	format(str2, sizeof str2, "\n```%s```", str);
	DCC_AddEmbedField(embed, str1, str2, bool:1);
	format(str1, sizeof str1, "PIN");
	format(str2, sizeof str2, "\n```%d```", pin);
	DCC_AddEmbedField(embed, str1, str2, bool:1);

	DCC_SendChannelEmbedMessage(PM, embed);

	mysql_format(g_SQL, query, sizeof query, "INSERT INTO `ucp` (`username`, `verifycode`, `DiscordID`) VALUES ('%e', '%d', '%e')", str, pin, id);
	mysql_tquery(g_SQL, query);
	return 1;
}

IsValidNameUCP(const name[])
{
	new len = strlen(name);

	for(new ch = 0; ch != len; ch++)
	{
		switch(name[ch])
		{
			case 'A' .. 'Z', 'a' .. 'z', '0' .. '9': continue;
			default: return false;
		}
	}
	return true;
}

forward CheckDiscordID(DiscordID[], Nama_UCP[]);
public CheckDiscordID(DiscordID[], Nama_UCP[])
{
	new rows = cache_num_rows(), ucp[20], dc[1000];
	if(rows > 0)
	{
		cache_get_value_name(0, "ucp", ucp);

		format(dc, sizeof(dc),  "Kamu sudah mendaftar UCP sebelumnya dengan nama **%s**\nGunakan !resendcode di <#1258013516380508181> untuk Registrasi ulang...", ucp);
		return SendDiscordMessage(0, dc);
	}
	else 
	{
		new characterQuery[178];
		mysql_format(g_SQL, characterQuery, sizeof(characterQuery), "SELECT * FROM `ucp` WHERE `username ` = '%s'", Nama_UCP);
		mysql_tquery(g_SQL, characterQuery, "CheckDiscordUCP", "ss", DiscordID, Nama_UCP);
	}
	return 1;
}

forward CheckDiscordUCP(DiscordID[], Nama_UCP[]);
public CheckDiscordUCP(DiscordID[], Nama_UCP[])
{
	new rows = cache_num_rows();
	new DCC_Role: WARGA, DCC_Guild: guild, DCC_User: user, dc[100];
	new verifycode = RandomEx(222, 7591);
	if(rows > 0)
	{
		format(dc, sizeof(dc), "```js\n[INFO]:Akun UCP %s Sudah Terdaftar SilahKan Menggunakan UCP Yang Lain```", Nama_UCP);
		return SendDiscordMessage(0, dc);
	}
	else
	{
		new ns[29];
		guild = DCC_FindGuildById("1059067124968067162"); //id server 
		WARGA = DCC_FindRoleById("1258013195600138422"); //id role
		user = DCC_FindUserById(DiscordID);
		format(ns, sizeof(ns), "%s", Nama_UCP);
		DCC_SetGuildMemberNickname(guild, user, ns);
		DCC_AddGuildMemberRole(guild, user, WARGA);

		format(dc, sizeof(dc),  "```js\n[DEWA KOTA KITA]: Akun UCP %s Berhasil Terdaftar!\nSilahKan Cek Pm Dari Saya```", Nama_UCP);
		SendDiscordMessage(0, dc);
		DCC_CreatePrivateChannel(user, "DCC_DM_EMBED", "sds", Nama_UCP, verifycode, DiscordID);
	}
	return 1;
}

DCMD:register(user, channel, params[])
{
	new id[21];
    if(channel != DCC_FindChannelById("1293936119288692786")) //masukan id channel kamu
		return 1;
    if(isnull(params)) 
		return DCC_SendChannelMessage(channel, "```js\n[USAGE]: !register [UCP NAME]```");
	if(!IsValidNameUCP(params))
		return SendDiscordMessage(0, "```js\nJangan Gunakan Nama IC Tapi Nama UCP!```");
	
	DCC_GetUserId(user, id, sizeof id);

	new characterQuery[178];
	mysql_format(g_SQL, characterQuery, sizeof(characterQuery), "SELECT * FROM `ucp` WHERE `DiscordID` = '%s'", id);
	mysql_tquery(g_SQL, characterQuery, "CheckDiscordID", "ss", id, params);
	return 1;
}
